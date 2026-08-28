#pragma once
#include <CosmicEngine/models/scene/scene.hpp>

#include <chrono>
#include <string>
#include <utility>
#include <vector>

namespace Bench
{
    // Benchmark driver. Behaviour is selected through environment variables:
    //   BENCH_MODE   startup | render | collision | scene | load | memory
    //   BENCH_N      number of objects (render/collision/scene/memory)
    //   BENCH_STRUCT grid | quadtree
    //   BENCH_CELL   grid cell size (default 64)
    //   BENCH_DEPTH  quadtree max depth (default 5)
    //   BENCH_NODE   quadtree max objects per node (default 4)
    //   BENCH_WARMUP seconds ignored before measuring (default 2)
    //   BENCH_MEASURE seconds measured (default 5)
    //   BENCH_SEED   RNG seed (default 42)
    //   BENCH_OUT    output CSV path (default bench_out.csv)
    class BenchScene : public CosmicEngine::Scene
    {
    public:
        BenchScene();
        void loadResources() override;
        void init() override;
        void update(double deltaTime) override;
        void draw() override;

    private:
        using Clock = std::chrono::steady_clock;

        void SpawnObjects();
        void SampleMemory(const std::string &label);
        void WriteCsv();
        void Finish();

        // configuration
        std::string mode, structType, outPath;
        int n = 0, cell = 64, depth = 5, node = 4;
        double warmup = 2.0, measure = 5.0;
        unsigned seed = 42;

        // runtime
        std::string renderer, glVersion;
        Clock::time_point start, lastDraw;
        bool firstDraw = true, measuring = false, finished = false;
        long long ticks = 0, frames = 0, measuredTicks = 0;

        // samples: (kind, value, extra)
        struct Row { std::string kind; double value; std::string extra; };
        std::vector<Row> rows;
        std::vector<std::pair<std::string, double>> loadTimes;
        double startupMs = -1.0, glfwFirstDraw = -1.0;
        class BenchObject *pairA = nullptr, *pairB = nullptr;
    };
}
