#ifndef NABAZU_SKYBOX_HPP
#define NABAZU_SKYBOX_HPP

namespace CosmicEngine
{
    class Shader;
}

namespace NaBazu
{
    // Procedural animated skybox: twinkling stars + drifting nebula/galaxy blobs,
    // entirely shader-generated (no textures, so it stays crisp at any resolution).
    // Not a CosmicEngine::Object -- a small self-contained render helper (same shape
    // as ChaseCamera), since it owns raw GL resources (VAO/VBO) and a hand-built
    // Shader rather than participating in the Object/Body lifecycle.
    //
    // Rendered as an "infinite" cube: the vertex shader uses the camera's view matrix
    // with translation stripped (rotation only) and outputs gl_Position = (proj*view*
    // pos).xyww, forcing depth = 1.0 so it is always drawn behind every real object
    // while still visually rotating with the camera as it turns through the rail's
    // curves (unlike a fixed full-screen quad, which would look "stuck to the screen").
    class Skybox
    {
    public:
        Skybox();
        ~Skybox();

        Skybox(const Skybox &) = delete;
        Skybox &operator=(const Skybox &) = delete;

        // elapsedTime drives star twinkle phase and slow nebula drift.
        void Draw(float elapsedTime) const;

    private:
        CosmicEngine::Shader *shader_;
        unsigned int vao_;
        unsigned int vbo_;
    };
}

#endif
