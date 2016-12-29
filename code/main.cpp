// Std. Includes
#include <string>
#include <cmath>

// GLEW
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// GL includes
#include "Shader.h"
#include "Model.h"
#include "Camera.h"

// GLM Mathemtics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "AprilTagReader.h"

// Properties

//void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

int main()
{
    GLuint screenWidth = 800, screenHeight = 600;
    GLuint imageWidth = 640, imageHeight = 480;

    
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "OpenGL", nullptr, nullptr); // Windowed
    glfwMakeContextCurrent(window);
  //  glfwSetKeyCallback(window, key_callback);

   
    glewExperimental = GL_TRUE;
    glewInit();
    
    // Define the viewport dimensions
    int width, height;
    // On recupere les dimensions de la fenetre creee plus haut
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    
    //glEnable(GL_DEPTH_TEST);
    
    Shader shader("opengl_code/shaders/default.vertexshader", "opengl_code/shaders/default.fragmentshader");
    
    AprilTagReader reader(0,imageWidth,imageHeight);
    
    cv::Mat image = reader.getImage();
    //use fast 4-byte alignment (default anyway) if possible
    //glPixelStorei(GL_UNPACK_ALIGNMENT, (image.step & 3) ? 1 : 4);
    
    //set length of one complete row in data (doesn't need to equal image.cols)
    //glPixelStorei(GL_UNPACK_ROW_LENGTH, image.step/image.elemSize());
    // Init GLFW
    cv::flip(image, image, 0);
    
    GLuint texture; // Declaration de l'identifiant

	glGenTextures(1, &texture); // Generation de la texture
	// On bind la texture cree dans le contexte global d'OpenGL
	glBindTexture(GL_TEXTURE_2D, texture); 
	// Modification des parametres de la texture
	// Methode de wrapping
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); 
	// Methode de filtrage
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Chargement du fichier image en utilisant la lib SOIL
	// Association des donnees image a la texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.cols, image.rows,
					           0, GL_BGR, GL_UNSIGNED_BYTE, image.ptr());
	// Generation de la mipmap
	glGenerateMipmap(GL_TEXTURE_2D);

    // On unbind la texture
	glBindTexture(GL_TEXTURE_2D, 0);
	
    shader.Use();
    glm::mat4 projection = glm::perspective((GLfloat) (43.13f*M_PI/180.0), (GLfloat) 1, 0.1f, 100.0f);
    
    glUniformMatrix4fv(glGetUniformLocation(shader.Program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    GLfloat focal = projection[0][0]; // f
    /*std::cout << focal << std::endl;
    std::cout << projection[1][1] << std::endl;
    std::cout << projection[3][3] << std::endl;
*/
    for(int i=0;i<4;++i)
    {
        for(int j=0;j<4;++j)
            std::cout << projection[j][i] << "\t";
        std::cout << std::endl;
    }
  




	GLfloat vertices[] = {
        
        /*     Positions    |      Normales     |     UV     */
       1.0f,  0.75f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // Top Right
       1.0f, -0.75f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // Bottom Right
      -1.0f, -0.75f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // Bottom Left
      -1.0f,  0.75f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // Top Left
    };
    
    GLshort indices[]{
    	0, 1, 3,
    	1, 2, 3
    };    
    
    GLuint VBO, VAO, EBO;
    
    glGenVertexArrays(1, &VAO);
    
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    glBindVertexArray(VAO);
    
    // On met notre EBO dans le contexte global
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    // On assigne au EBO le tableau d'indices
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); 

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Attribut des positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    // Attribut des normales
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3*sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    // Attribut des coord
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6*sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    shader.Use();

    Model suzanne("opengl_code/model/suzanne.obj");
    // Game loop
    Camera camera(window,glm::vec3(0.0f,0.0f,focal));
    while(!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        camera.Do_Movement();
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT /*| GL_DEPTH_BUFFER_BIT*/);

        shader.Use();
        //glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, -focal), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 view = camera.GetViewMatrix();
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "view"), 1, GL_FALSE, glm::value_ptr(view));
        
        glm::mat4 model;
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));	
        glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
		
        
        
        image = reader.getImage();
        cv::flip(image, image, 0);
        
		//  Activiation  de la  texture 0
		glActiveTexture(GL_TEXTURE0 );//  Binding  de  notre  texture
		glBindTexture(GL_TEXTURE_2D , texture );//  Association du numero de la texture  pour le  shader
        
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.cols, image.rows,
                     0, GL_BGR, GL_UNSIGNED_BYTE, image.ptr());

		glUniform1i(glGetUniformLocation(shader.Program , "modelTexture"), 0);
        
        glBindVertexArray(VAO);
        // On dessine l'objet courant 
        glDrawElements(GL_TRIANGLES, 3*2, GL_UNSIGNED_SHORT, 0);
        glBindVertexArray(0);

     

        posT tmat = reader.getTags();
        if(tmat.size()){
            // Affichage de la 3D
            std::pair<dvec3, dvec3x3> tag0transform = tmat[0];
            model=glm::mat4(1.0f);
            

            
            GLfloat scaleNorm = sqrt(pow(tag0transform.first[0],2) + pow(tag0transform.first[1],2) + pow(tag0transform.first[2],2))/focal;
            GLfloat scaleFactor = ((focal-0.1)/100*scaleNorm+focal+(0.1-focal)*3/2);
            //std::cout << scaleNorm << std::endl;
            //std::cout << scaleFactor << std::endl;

            //model=glm::translate(model, glm::vec3(tag0transform.first[0]/scaleNorm/scaleFactor, -tag0transform.first[1]/scaleNorm/scaleFactor, -(tag0transform.first[2])/scaleNorm/scaleFactor));
            model=glm::translate(model, glm::vec3(tag0transform.first[0], -tag0transform.first[1], -tag0transform.first[2]));
            
            //model=glm::translate(model, glm::vec3(0,0,-focal/2));
            
            glm::mat4 rot(1.0f);
            for(int i=0;i<3;++i){
                for(int j=0;j<3;++j)
                    model[i][j]=tag0transform.second[i][j];
            }
            //model=rot*model;
            //model=glm::scale(model, glm::vec3(scaleFactor, scaleFactor, scaleFactor));
            //model=glm::scale(model, glm::vec3(scaleFactor, scaleFactor, scaleFactor));
            glUniformMatrix4fv(glGetUniformLocation(shader.Program, "model"), 1, GL_FALSE, glm::value_ptr(model));
            suzanne.Draw(shader);

        }

        glfwSwapBuffers(window);
    }
    
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    
    glfwTerminate();
    return 0;
}

/*
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}
*/