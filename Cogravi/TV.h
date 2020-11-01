#ifndef TV_HEADER
#define TV_HEADER

#include <opencv2/opencv.hpp>
#include <opencv2/video.hpp>

using namespace cv;

namespace Cogravi
{
	class TV
	{
	public:

		GameObject* object;
		VideoCapture captura;
		Mat frame;
		Mat img;
		Shader ourShader;
		VideoCapture cap;
		unsigned int VBO, VAO;
		unsigned int texture1, texture2;

		TV(const char *name)
		{
			object = new GameObject(glm::vec3(1.0f), glm::vec3(1.0f), glm::vec3(1.0f), "assets/objects/televisor/televisor.obj");
			captura =  VideoCapture(name);			   
			ourShader = Shader("assets/shaders/practica.vert", "assets/shaders/practica.frag");

            float vertices[] = {
               -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
                0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
                0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
                0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
               -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
               -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

               -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
                0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
                0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
                0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
               -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
               -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

               -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
               -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
               -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
               -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
               -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
               -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

                0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
                0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
                0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
                0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
                0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
                0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

               -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
                0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
                0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
                0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
               -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
               -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

               -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
                0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
                0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
                0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
               -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
               -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
            };

            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);

            glBindVertexArray(VAO);

            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

            // position attribute
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);
            // texture coord attribute
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
            glEnableVertexAttribArray(1);

            glGenTextures(1, &texture1);
            glBindTexture(GL_TEXTURE_2D, texture1);
            // set the texture wrapping parameters
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            // set texture filtering parameters
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            // load image, create texture and generate mipmaps
            int width, height, nrChannels;
            stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
            unsigned char* data = stbi_load("assets/textures/container.jpg", &width, &height, &nrChannels, 0);
            if (data)
            {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
                glGenerateMipmap(GL_TEXTURE_2D);
            }
            else
            {
                std::cout << "Failed to load texture" << std::endl;
            }
            stbi_image_free(data);
            // texture 2
            // ---------
            glGenTextures(1, &texture2);
            glBindTexture(GL_TEXTURE_2D, texture2);
            // set the texture wrapping parameters
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            // set texture filtering parameters
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            // load image, create texture and generate mipmaps
            data = stbi_load("assets/textures/awesomeface.png", &width, &height, &nrChannels, 0);
            if (data)
            {
                // note that the awesomeface.png has transparency and thus an alpha channel, so make sure to tell OpenGL the data type is of GL_RGBA
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
                glGenerateMipmap(GL_TEXTURE_2D);
            }
            else
            {
                std::cout << "Failed to load texture" << std::endl;
            }
            stbi_image_free(data);


            ourShader.use();
            ourShader.setInt("texture1", 0);
            ourShader.setInt("texture2", 1);

            //cap = VideoCapture("goku.mp4");
            //if (!cap.isOpened())  // isOpened() returns true if capturing has been initialized.
            //{

            //    return ;
            //}

            int rows = 600;
            int cols = 800;
            int rec = 150;


            //Mat img = imread("c:/data/lena.jpg");
            img=Mat (rows, cols, CV_8UC3, Scalar::all(0));

            //dibuja un circulo en el centro de la imagen de un radio de 250
            //circle(img, Point(cols / 2, rows / 2), 250, Scalar(255, 0, 0), 3);

            ////dibuja una linea
            //line(img, Point(), Point(cols, rows), CV_RGB(255, 0, 0), 2);

            ////dibuja un rectangulo
            //rectangle(img, Point(rec, rec), Point(cols - rec, rows - rec), CV_RGB(0, 255, 255));

            //dibuja el texto Opencv 2
            char textop[100] = "Yordy Leonidas";
            putText(img, textop, Point(180, 320), FONT_HERSHEY_SCRIPT_COMPLEX, 2, CV_RGB(125, 255, 145));

            ourShader.use(); // don't forget to activate/use the shader before setting uniforms!
           // either set it manually like so:
            glUniform1i(glGetUniformLocation(ourShader.ID, "texture1"), 0);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img.cols, img.rows, 0, GL_BGR, GL_UNSIGNED_BYTE, img.ptr());

		}

		void render(Camera& camera)
		{
            //if (!cap.read(frame))
            //{
            //    //delete cap;
            //    cap = VideoCapture("goku.mp4");
            //    // cap is the object of class video capture that tries to capture Bumpy.mp4
            //    if (!cap.isOpened())  // isOpened() returns true if capturing has been initialized.
            //    {

            //        return;
            //    }
            //}

            // bind textures on corresponding texture units
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture1);
            //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img.cols, img.rows, 0, GL_BGR, GL_UNSIGNED_BYTE, img.ptr());
            //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, frame.cols, frame.rows, 0, GL_BGR, GL_UNSIGNED_BYTE, frame.ptr());
            //glActiveTexture(GL_TEXTURE1);
            //glBindTexture(GL_TEXTURE_2D, texture2);

            // activate shader
            ourShader.use();

            // create transformations
            glm::mat4 view = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
            glm::mat4 projection = glm::mat4(1.0f);
            projection = camera.GetProjectionMatrix();
            view = camera.GetViewMatrix();
            // pass transformation matrices to the shader
            ourShader.setMat4("projection", projection); // note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.
            ourShader.setMat4("view", view);
       
            glBindVertexArray(VAO);

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(0, 1, 0));
            float angle = 0.0f;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            ourShader.setMat4("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);    
            glBindVertexArray(0);
            glActiveTexture(GL_TEXTURE0);
		
		}	

	};
}
#endif
