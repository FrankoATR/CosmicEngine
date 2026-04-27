#ifndef COSMIC_MASTERSHADERS_HPP
#define COSMIC_MASTERSHADERS_HPP

/**
 * @file master_shaders.hpp
 * @brief Provides built-in shader source strings used by the resource manager.
 */


#if GAME_MODE_CONFIGURATION == GAME_2D_CONFIGURATION
    /** @brief Default 2D sprite vertex shader source (textured quad with model/projection). */
    const char *sprite_vs = 
    "#version 330 core\n"
    "layout (location = 0) in vec4 vertex;\n"
    "out vec2 TexCoords;\n"
    "uniform mat4 model;\n"
    "uniform mat4 projection;\n"
    "void main()\n"
    "{\n"
    "TexCoords = vertex.zw;\n"
    "gl_Position = projection * model * vec4(vertex.xy, 0.0, 1.0);\n"
    "}\0";

    /** @brief Default 2D sprite fragment shader source (samples a texture and tints by spriteColor). */
    const char *sprite_fs = 
    "#version 330 core\n"
    "in vec2 TexCoords;\n"
    "out vec4 color;\n"
    "uniform sampler2D image;\n"
    "uniform vec3 spriteColor;\n"
    "void main()\n"
    "{\n"
    "color = vec4(spriteColor, 1.0) * texture(image, TexCoords);\n"
    "}\0";

    /** @brief Sprite-sheet vertex shader source (remaps UVs through uvMin / uvMax). */
    const char *spriteSheet_vs = 
    "#version 330 core\n"
    "layout (location = 0) in vec4 vertex;\n"
    "out vec2 TexCoords;\n"
    "uniform mat4 model;\n"
    "uniform mat4 projection;\n"
    "uniform vec2 uvMin;\n"
    "uniform vec2 uvMax;\n"
    "void main()\n"
    "{\n"
    "TexCoords = mix(uvMin, uvMax, vertex.zw);\n"
    "gl_Position = projection * model * vec4(vertex.xy, 0.0, 1.0);\n"
    "}\0";

    /** @brief Sprite-sheet fragment shader source (samples the active frame and tints by spriteColor). */
    const char *spriteSheet_fs = 
    "#version 330 core\n"
    "in vec2 TexCoords;\n"
    "out vec4 color;\n"
    "uniform sampler2D image;\n"
    "uniform vec3 spriteColor;\n"
    "void main()\n"
    "{\n"
    "color = vec4(spriteColor, 1.0) * texture(image, TexCoords);\n"
    "}\0";

    /** @brief 2D primitive vertex shader source used by the PrimitiveManager. */
    const char *shape_2D_vs = 
    "#version 330 core\n"
    "layout(location = 0) in vec3 aPos;\n"
    "uniform mat4 model;\n"
    "uniform mat4 projection;\n"
    "void main()\n"
    "{\n"
    "gl_Position = projection * model * vec4(aPos, 1.0);\n"
    "}\0";

    /** @brief 2D primitive fragment shader source (solid LineColor). */
    const char *shape_2D_fs = 
    "#version 330 core\n"
    "uniform vec4 LineColor;\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "FragColor = LineColor;\n"
    "}\0";

    /** @brief Bitmap-font vertex shader source used by FontManager. */
    const char *text_vs =
    "#version 330 core\n"
    "layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>\n"
    "out vec2 TexCoords;\n"
    "uniform mat4 projection;\n"
    "void main()\n"
    "{\n"
    "gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);\n"
    "TexCoords = vertex.zw;\n"
    "}\0";

    /** @brief Bitmap-font fragment shader source (alpha sampled from glyph atlas, tinted by textColor). */
    const char *text_fs =
    "#version 330 core\n"
    "in vec2 TexCoords;\n"
    "out vec4 color;\n"
    "uniform sampler2D text;\n"
    "uniform vec3 textColor;\n"
    "void main()\n"
    "{\n"
    "vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);\n"
    "color = vec4(textColor, 1.0) * sampled;\n"
    "}\0";


#elif GAME_MODE_CONFIGURATION == GAME_3D_CONFIGURATION
/** @brief Default 2D-billboard sprite vertex shader source for 3D mode (orthographic overlay). */
const char *sprite_vs = 
"#version 330 core\n"
"layout (location = 0) in vec4 vertex;\n"
"out vec2 TexCoords;\n"
"uniform mat4 model;\n"
"uniform mat4 projection;\n"
"void main()\n"
"{\n"
"TexCoords = vertex.zw;\n"
"gl_Position = projection * model * vec4(vertex.xy, 0.0, 1.0);\n"
"}\0";

/** @brief Default 2D-billboard sprite fragment shader source for 3D mode. */
const char *sprite_fs = 
"#version 330 core\n"
"in vec2 TexCoords;\n"
"out vec4 color;\n"
"uniform sampler2D image;\n"
"uniform vec3 spriteColor;\n"
"void main()\n"
"{\n"
"color = vec4(spriteColor, 1.0) * texture(image, TexCoords);\n"
"}\0";

/** @brief Sprite-sheet vertex shader source for 3D mode (animated frames in HUD layer). */
const char *spriteSheet_vs = 
"#version 330 core\n"
"layout (location = 0) in vec4 vertex;\n"
"out vec2 TexCoords;\n"
"uniform mat4 model;\n"
"uniform mat4 projection;\n"
"uniform vec2 uvMin;\n"
"uniform vec2 uvMax;\n"
"void main()\n"
"{\n"
"TexCoords = mix(uvMin, uvMax, vertex.zw);\n"
"gl_Position = projection * model * vec4(vertex.xy, 0.0, 1.0);\n"
"}\0";

/** @brief Sprite-sheet fragment shader source for 3D mode. */
const char *spriteSheet_fs = 
"#version 330 core\n"
"in vec2 TexCoords;\n"
"out vec4 color;\n"
"uniform sampler2D image;\n"
"uniform vec3 spriteColor;\n"
"void main()\n"
"{\n"
"color = vec4(spriteColor, 1.0) * texture(image, TexCoords);\n"
"}\0";


/** @brief 2D primitive vertex shader source used inside the 3D HUD pass. */
const char *shape_2D_vs = 
"#version 330 core\n"
"layout(location = 0) in vec3 aPos;\n"
"uniform mat4 model;\n"
"uniform mat4 projection;\n"
"uniform mat4 view;\n"
"void main()\n"
"{\n"
"gl_Position = projection * model * vec4(aPos, 1.0);\n"
"}\0";

/** @brief 2D primitive fragment shader source used inside the 3D HUD pass. */
const char *shape_2D_fs = 
"#version 330 core\n"
"uniform vec4 LineColor;\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"FragColor = LineColor;\n"
"}\0";


/** @brief 3D primitive vertex shader source used by PrimitiveManager (model/view/projection). */
const char *shape_3D_vs = 
"#version 330 core\n"
"layout(location = 0) in vec3 aPos;\n"
"uniform mat4 model;\n"
"uniform mat4 projection;\n"
"uniform mat4 view;\n"
"void main()\n"
"{\n"
"gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
"}\0";

/** @brief 3D primitive fragment shader source (solid LineColor). */
const char *shape_3D_fs = 
"#version 330 core\n"
"uniform vec4 LineColor;\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"FragColor = LineColor;\n"
"}\0";

/** @brief Bitmap-font vertex shader source used by FontManager in 3D mode. */
const char *text_vs =
"#version 330 core\n"
"layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>\n"
"out vec2 TexCoords;\n"
"uniform mat4 projection;\n"
"void main()\n"
"{\n"
"gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);\n"
"TexCoords = vertex.zw;\n"
"}\0";

/** @brief Bitmap-font fragment shader source used by FontManager in 3D mode. */
const char *text_fs =
"#version 330 core\n"
"in vec2 TexCoords;\n"
"out vec4 color;\n"
"uniform sampler2D text;\n"
"uniform vec3 textColor;\n"
"void main()\n"
"{\n"
"vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);\n"
"color = vec4(textColor, 1.0) * sampled;\n"
"}\0";




/** @brief 3D model vertex shader source (Phong-style lighting prep). */
const char *model_3d_vs = R"glsl(
    #version 330 core
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aNormal;
    layout (location = 2) in vec2 aTexCoords;

    out vec3 FragPos;
    out vec3 Normal;
    out vec2 TexCoords;

    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;

    void main()
    {
        FragPos = vec3(model * vec4(aPos, 1.0));
        Normal = mat3(transpose(inverse(model))) * aNormal;  
        TexCoords = aTexCoords;
        
        gl_Position = projection * view * vec4(FragPos, 1.0);
    }
)glsl";


/** @brief 3D model fragment shader source (Material + DirLight/PointLight/SpotLight). */
const char *model_3d_fs = R"glsl(
    #version 330 core
    out vec4 FragColor;
    
    struct Material {
        sampler2D diffuse;
        sampler2D specular;
        float shininess;
    }; 
    
    struct DirLight {
        vec3 direction;
        
        vec3 ambient;
        vec3 diffuse;
        vec3 specular;
    };
    
    struct PointLight {
        vec3 position;
        
        float constant;
        float linear;
        float quadratic;
        
        vec3 ambient;
        vec3 diffuse;
        vec3 specular;
    };
    
    struct SpotLight {
        vec3 position;
        vec3 direction;
        float cutOff;
        float outerCutOff;
      
        float constant;
        float linear;
        float quadratic;
      
        vec3 ambient;
        vec3 diffuse;
        vec3 specular;       
    };
    
    #define NR_POINT_LIGHTS 4
    
    in vec3 FragPos;
    in vec3 Normal;
    in vec2 TexCoords;
    
    uniform vec3 viewPos;
    uniform DirLight dirLight;
    uniform PointLight pointLights[NR_POINT_LIGHTS];
    uniform SpotLight spotLight;
    uniform Material material;
    
    vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
    vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
    vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
    
    void main()
    {    
        vec3 norm = normalize(Normal);
        vec3 viewDir = normalize(viewPos - FragPos);
        
        vec3 result = CalcDirLight(dirLight, norm, viewDir);
        for(int i = 0; i < NR_POINT_LIGHTS; i++)
            result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
        //result += CalcSpotLight(spotLight, norm, FragPos, viewDir);    
        
        FragColor = vec4(result, 1.0);
        //FragColor = texture(material.diffuse, TexCoords); // constant light 1.0

    }
    
    vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
    {
        vec3 lightDir = normalize(-light.direction);
        float diff = max(dot(normal, lightDir), 0.0);
        vec3 reflectDir = reflect(-lightDir, normal);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
        vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
        vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
        return (ambient + diffuse + specular);
    }
    
    vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
    {
        vec3 lightDir = normalize(light.position - fragPos);
        float diff = max(dot(normal, lightDir), 0.0);
        vec3 reflectDir = reflect(-lightDir, normal);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        float distance = length(light.position - fragPos);
        float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
        vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
        vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
        vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
        ambient *= attenuation;
        diffuse *= attenuation;
        specular *= attenuation;
        return (ambient + diffuse + specular);
    }
    
    vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
    {
        vec3 lightDir = normalize(light.position - fragPos);
        float diff = max(dot(normal, lightDir), 0.0);
        vec3 reflectDir = reflect(-lightDir, normal);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
        float distance = length(light.position - fragPos);
        float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
        float theta = dot(lightDir, normalize(-light.direction)); 
        float epsilon = light.cutOff - light.outerCutOff;
        float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
        vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
        vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
        vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));
        ambient *= attenuation * intensity;
        diffuse *= attenuation * intensity;
        specular *= attenuation * intensity;
        return (ambient + diffuse + specular);
    }
    )glsl";


#else
    #error "[ObjectManager] You must choose a game mode configuration (GAME_2D_CONFIGURATION Or GAME_3D_CONFIGURATION)"
#endif





#endif // COSMIC_MASTERSHADERS_HPP