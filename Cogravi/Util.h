#ifndef UTIL_HEADER
#define UTIL_HEADER

#include <map>
#include <vector>

namespace Cogravi {

    enum ColliderType 
    {
        MESH,
        BOX,
        SPHERE,
        CAPSULE,
        CYLINDER,
        CONE
    };

    enum ShaderType
    {
        MODEL_STATIC,
        MODEL_DYNAMIC,
        MODEL_DYNAMIC_1,
        TERRAIN,
        SKYBOX,
        CUBE_MAP,
        AVATAR
    };

    enum TextureType
    {
        DIFFUSE,
        SPECULAR,
        NORMAL
    };

    struct Texture {
        GLuint id;
        TextureType type;
        string path;
        Texture() {}
        Texture(GLuint id, TextureType type)
        {
            this->id = id;
            this->type = type;
        }
    };

    class Util
    {

    public:

        static Util* Instance()
        {
            static Util instance;
            return &instance;
        }

        map<ShaderType, Shader*> myShaders;
        map<TextureType, string> typeTexture;

        Util()
        {
            inicializarShaders();
            loadTextureType();
        }

        void inicializarShaders()
        {
            myShaders[ShaderType::MODEL_STATIC] = new Shader("assets/shaders/model.vert", "assets/shaders/model.frag");
            myShaders[ShaderType::MODEL_DYNAMIC] = new Shader("assets/shaders/animation.vert", "assets/shaders/animation.frag");
            myShaders[ShaderType::MODEL_DYNAMIC_1] = new Shader("assets/shaders/animation1.vert", "assets/shaders/animation1.frag");
            myShaders[ShaderType::TERRAIN] = new Shader("assets/shaders/terrain.vert", "assets/shaders/terrain.frag");      
            myShaders[ShaderType::SKYBOX] = new Shader("assets/shaders/skybox.vert", "assets/shaders/skybox.frag");
            myShaders[ShaderType::CUBE_MAP] = new Shader("assets/shaders/cubemaps.vert", "assets/shaders/cubemaps.frag");
            myShaders[ShaderType::AVATAR] = new Shader("assets/shaders/avatar.vert", "assets/shaders/avatar.frag");
        }

        void loadTextureType()
        {
            typeTexture[TextureType::DIFFUSE] = "diffuse";
            typeTexture[TextureType::NORMAL] = "normal";
            typeTexture[TextureType::SPECULAR] = "specular";
        }

        // -------------------------------------------------------
       // order:
       // +X (right)
       // -X (left)
       // +Y (top)
       // -Y (bottom)
       // +Z (front) 
       // -Z (back)
       // -------------------------------------------------------

        static GLuint loadCubemap(vector<std::string> faces)
        {
            GLuint textureID;
            glGenTextures(1, &textureID);
            glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

            int width, height, nrComponents;
            for (GLuint i = 0; i < faces.size(); i++)
            {
                unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrComponents, 0);
                if (data)
                {
                    GLenum format;
                    if (nrComponents == 1)
                        format = GL_RED;
                    else if (nrComponents == 3)
                        format = GL_RGB;
                    else if (nrComponents == 4)
                        format = GL_RGBA;
                    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
                    stbi_image_free(data);
                }
                else
                {
                    std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
                    stbi_image_free(data);
                }
            }
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

            return textureID;
        }

        static GLuint loadTexture(char const* path, bool gammaCorrection = false)
        {
            GLuint textureID;
            glGenTextures(1, &textureID);

            int width, height, nrChannels;
            unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
            if (data)
            {
                GLenum internalFormat;
                GLenum dataFormat;
                if (nrChannels == 1)
                    internalFormat = dataFormat = GL_RED;
                else if (nrChannels == 3)
                {
                    internalFormat = gammaCorrection ? GL_SRGB : GL_RGB;
                    dataFormat = GL_RGB;
                }
                else if (nrChannels == 4)
                {
                    internalFormat = gammaCorrection ? GL_SRGB_ALPHA : GL_RGBA;
                    dataFormat = GL_RGBA;
                }

                glBindTexture(GL_TEXTURE_2D, textureID);
                glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
                glGenerateMipmap(GL_TEXTURE_2D);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

                stbi_image_free(data);
            }
            else
            {
                std::cout << "Failed to load texture: " << path << std::endl;
                stbi_image_free(data);
            }

            return textureID;
        }


        static GLuint TextureFromFile(const char* path, const string& directory)
        {
            string filename = string(path);
            filename = directory + '/' + filename;

            GLuint textureID;
            glGenTextures(1, &textureID);

            int width, height, nrComponents;
            unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
            if (data)
            {
                GLenum format;
                if (nrComponents == 1)
                    format = GL_RED;
                else if (nrComponents == 3)
                    format = GL_RGB;
                else if (nrComponents == 4)
                    format = GL_RGBA;

                glBindTexture(GL_TEXTURE_2D, textureID);
                glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
                glGenerateMipmap(GL_TEXTURE_2D);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

                stbi_image_free(data);
            }
            else
            {
                std::cout << "Texture failed to load at path: " << path << std::endl;
                stbi_image_free(data);
            }

            return textureID;
        }



      
    };
}
#endif