#include <glad/glad.h>
#include "bench_scene.hpp"

#include "../entities/bench_object.hpp"
#include "../utilities/bench_state.hpp"
#include "../utilities/sysinfo.hpp"

#include <CosmicEngine/interfaces/definitions.hpp>
#include <CosmicEngine/managers/game_manager.hpp>
#include RESOURCEMANAGER_HEADER
#include OBJECTMANAGER_HEADER
#include BODYMANAGER_HEADER
#include <CosmicEngine/collisions/collision_area.hpp>
#include <GLFW/glfw3.h>

#include <cstdio>
#include <fstream>
#include <random>

namespace Bench
{
    namespace
    {
        double Ms(std::chrono::steady_clock::time_point a, std::chrono::steady_clock::time_point b)
        {
            return std::chrono::duration<double, std::milli>(b - a).count();
        }
    }

    BenchScene::BenchScene() : CosmicEngine::Scene("BenchScene")
    {
        mode       = GetEnvOr("BENCH_MODE", "render");
        n          = std::stoi(GetEnvOr("BENCH_N", "1000"));
        structType = GetEnvOr("BENCH_STRUCT", "grid");
        cell       = std::stoi(GetEnvOr("BENCH_CELL", "64"));
        depth      = std::stoi(GetEnvOr("BENCH_DEPTH", "5"));
        node       = std::stoi(GetEnvOr("BENCH_NODE", "4"));
        warmup     = std::stod(GetEnvOr("BENCH_WARMUP", "2"));
        measure    = std::stod(GetEnvOr("BENCH_MEASURE", "5"));
        seed       = static_cast<unsigned>(std::stoul(GetEnvOr("BENCH_SEED", "42")));
        outPath    = GetEnvOr("BENCH_OUT", "bench_out.csv");
    }

    void BenchScene::loadResources()
    {
        if (mode == "load")
        {
            const char *textures[] = {
                "noise_256_0", "noise_256_1", "noise_256_2",
                "noise_512_0", "noise_512_1", "noise_512_2",
                "noise_1024_0", "noise_1024_1", "noise_1024_2",
                "noise_2048_0", "noise_2048_1", "noise_2048_2"};
            for (const char *key : textures)
            {
                const std::string path = std::string("assets/textures/") + key + ".png";
                const auto t0 = Clock::now();
                RS_MN.LoadTexture(key, path);
                loadTimes.emplace_back(key, Ms(t0, Clock::now()));
            }
            const auto f0 = Clock::now();
            RS_MN.LoadFont("bench_font", "assets/fonts/ThaleahFat.ttf", 32);
            loadTimes.emplace_back("font_32", Ms(f0, Clock::now()));
            return;
        }
        if (mode == "render" || mode == "scene" || mode == "memory")
        {
            RS_MN.LoadTexture("sprite", "assets/textures/sprite.png");
        }
    }

    void BenchScene::init()
    {
        GM_MN.disableVsync();
        GM_MN.setTargetFps(1000000.0); // uncapped presentation: frame time is the measurement

        const GLubyte *r = glGetString(GL_RENDERER);
        const GLubyte *v = glGetString(GL_VERSION);
        renderer  = r ? reinterpret_cast<const char *>(r) : "unknown";
        glVersion = v ? reinterpret_cast<const char *>(v) : "unknown";

        SampleMemory("baseline");

        const bool needsBodies = (mode == "collision" || mode == "scene" || mode == "memory" || mode == "pairtest");
        if (needsBodies)
        {
            const CosmicEngine::CollisionType type = (structType == "quadtree")
                ? CosmicEngine::CollisionType::QuadTree
                : CosmicEngine::CollisionType::Grid;
            BOD_MN.CreateCollisionArea(type, areaMin, areaMax - areaMin, cell, depth, node);
        }
        if (mode == "pairtest")
        {
            // Two fully overlapping static bodies in the middle of the area.
            const Vec size(16.0f);
            const Vec pos = (areaMin + areaMax) * 0.5f;
            pairA = new BenchObject(pos, size, Vec(0.0f), true, false);
            pairB = new BenchObject(pos, size, Vec(0.0f), true, false);
            OBJ_MN.Add(pairA);
            OBJ_MN.Add(pairB);
        }
        else if (mode != "startup" && mode != "load")
        {
            SpawnObjects();
        }
        SampleMemory("after_spawn");

        start = Clock::now();
        lastDraw = start;
    }

    void BenchScene::SpawnObjects()
    {
        std::mt19937 rng(seed);
        std::uniform_real_distribution<float> unit(0.0f, 1.0f);
        std::uniform_real_distribution<float> speed(50.0f, 150.0f);

        const bool withBody   = (mode == "collision" || mode == "scene" || mode == "memory");
        const bool withSprite = (mode == "render" || mode == "scene" || mode == "memory");
        const Vec size(16.0f);

        for (int i = 0; i < n; ++i)
        {
            Vec pos, vel;
            for (int axis = 0; axis < static_cast<int>(Vec::length()); ++axis)
            {
                pos[axis] = areaMin[axis] + unit(rng) * (areaMax[axis] - areaMin[axis] - size[axis]);
                vel[axis] = unit(rng) * 2.0f - 1.0f;
            }
            const float len = glm::length(vel);
            vel = (len > 1e-4f ? vel / len : Vec(1.0f)) * speed(rng);
            OBJ_MN.Add(new BenchObject(pos, size, vel, withBody, withSprite));
        }
    }

    void BenchScene::update(double)
    {
        if (finished) return;
        ++ticks;
        // Keep the measurement window alive from the tick side too, so that a
        // saturated loop (no presentation) still records collision-phase samples.
        {
            const double elapsedTick = Ms(start, Clock::now()) / 1000.0;
            measuring = (elapsedTick >= warmup) && (elapsedTick < warmup + measure);
            if (elapsedTick >= warmup + measure + 20.0) { Finish(); return; }
        }

        if (mode == "pairtest")
        {
            // One explicit pass; report how many callbacks each body received.
            collisionCount = 0;
            countCollisions = true;
            BOD_MN.update();
            countCollisions = false;
            rows.push_back({"pair_callbacks", static_cast<double>(pairA->OwnCallbacks()), "A"});
            rows.push_back({"pair_callbacks", static_cast<double>(pairB->OwnCallbacks()), "B"});
            rows.push_back({"pair_callbacks", static_cast<double>(collisionCount), "total"});
            Finish();
            return;
        }

        if (mode == "collision")
        {
            // Explicit timed pass over the collision phase. The engine runs its own
            // pass in the same tick; this second pass is identical work and is the
            // one we measure, so that the number excludes object updates and rendering.
            collisionCount = 0;
            countCollisions = true;
            const auto t0 = Clock::now();
            BOD_MN.update();
            const auto t1 = Clock::now();
            countCollisions = false;
            if (measuring)
            {
                rows.push_back({"tick_us", Ms(t0, t1) * 1000.0, std::to_string(collisionCount)});
                ++measuredTicks;
            }
        }
    }

    void BenchScene::draw()
    {
        if (finished) return;
        const auto now = Clock::now();

        if (firstDraw)
        {
            firstDraw = false;
            startupMs = ProcessAgeMs();
            glfwFirstDraw = glfwGetTime() * 1000.0;
            lastDraw = now;
            if (mode == "startup" || mode == "load")
            {
                Finish();
            }
            return;
        }

        ++frames;
        const double elapsed = Ms(start, now) / 1000.0;
        const bool inWindow = (elapsed >= warmup) && (elapsed < warmup + measure);
        if (inWindow && measuring)
        {
            rows.push_back({"frame_ms", Ms(lastDraw, now), ""});
        }
        measuring = inWindow;
        lastDraw = now;

        if (elapsed >= warmup + measure)
        {
            if (mode == "memory")
            {
                for (int i = 0; i < 5; ++i) SampleMemory("steady");
            }
            Finish();
        }
    }

    void BenchScene::SampleMemory(const std::string &label)
    {
        const MemInfo m = QueryMemory();
        rows.push_back({"mem_ws_bytes", static_cast<double>(m.workingSet), label});
        rows.push_back({"mem_private_bytes", static_cast<double>(m.privateBytes), label});
        rows.push_back({"mem_peak_ws_bytes", static_cast<double>(m.peakWorkingSet), label});
    }

    void BenchScene::WriteCsv()
    {
        std::ofstream f(outPath);
        f << "# mode=" << mode << ",n=" << n << ",struct=" << structType
          << ",cell=" << cell << ",depth=" << depth << ",node=" << node
          << ",seed=" << seed << ",warmup_s=" << warmup << ",measure_s=" << measure
#if GAME_MODE_CONFIGURATION == GAME_3D_CONFIGURATION
          << ",config=3D"
#else
          << ",config=2D"
#endif
          << ",ticks=" << ticks << ",frames=" << frames << ",measured_ticks=" << measuredTicks
          << ",startup_ms=" << startupMs << ",glfw_first_draw_ms=" << glfwFirstDraw
          << ",renderer=" << renderer << ",gl=" << glVersion << "\n";
        f << "kind,value,extra\n";
        f << "startup_ms," << startupMs << ",process_to_first_frame\n";
        f << "glfw_ms," << glfwFirstDraw << ",glfw_init_to_first_frame\n";
        for (const auto &lt : loadTimes)
        {
            f << "load_ms," << lt.second << "," << lt.first << "\n";
        }
        for (const Row &r : rows)
        {
            f << r.kind << "," << r.value << "," << r.extra << "\n";
        }
    }

    void BenchScene::Finish()
    {
        finished = true;
        WriteCsv();
        std::printf("[Bench] mode=%s n=%d struct=%s rows=%zu -> %s\n",
                    mode.c_str(), n, structType.c_str(), rows.size(), outPath.c_str());
        GM_MN.endprogram();
    }
}
