#ifndef MESH_M_HEADER
#define MESH_M_HEADER

namespace Cogravi {

    struct Vertex
    {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 TexCoords;
        glm::vec3 Tangent;
        glm::vec3 Bitangent;
    };

    class MeshM
    {
    public:

        vector<Vertex> vertices;
        vector<GLuint> indices;
        vector<Texture> textures;
        GLuint VAO, VBO, EBO;

        MeshM(vector<Vertex> vertices, vector<GLuint> indices, vector<Texture> &textures)
        {
            this->vertices = vertices;
            this->indices = indices;
            this->textures = textures;

            setupMesh();
        }

        ~MeshM()
        {

        }

        void draw(Shader &shader)
        {
            for (unsigned int i = 0; i < textures.size(); i++)
            {
                glActiveTexture(GL_TEXTURE0 + i);
                TextureType type = textures[i].type;
                glUniform1i(glGetUniformLocation(shader.ID, ("material." + Util::Instance()->typeTexture[type]).c_str()), i);

                glBindTexture(GL_TEXTURE_2D, textures[i].id);
            }

            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);

            glActiveTexture(GL_TEXTURE0);
        }

        void drawInstance(Shader &shader, int amount)
        {
            for (unsigned int i = 0; i < textures.size(); i++)
            {
                glActiveTexture(GL_TEXTURE0 + i);
                TextureType type = textures[i].type;
                glUniform1i(glGetUniformLocation(shader.ID, "texture_diffuse1"), i);

                glBindTexture(GL_TEXTURE_2D, textures[i].id);
            }
            glBindVertexArray(VAO);
            glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0, amount);
            glBindVertexArray(0);
        }

    private:

        void setupMesh()
        {

            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
            glGenBuffers(1, &EBO);

            glBindVertexArray(VAO);

            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

            // set the vertex attribute pointers
            // vertex Positions
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
            // vertex normals
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
            // vertex texture coords
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
            // vertex tangent
            glEnableVertexAttribArray(3);
            glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
            // vertex bitangent
            glEnableVertexAttribArray(4);
            glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));

            glBindVertexArray(0);
        }

    };
}
#endif