#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/intersect.hpp> // For ray-box intersection

#include <ft2build.h>  //for text rendering
#include FT_FREETYPE_H

#include "shader_s.h"
#include "camera.h"
#include "object_selection.h"
#include "model.h"
#include "text.h"
#include "inventory.h"

#include <iostream>
#include <map>
#include <string>


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

unsigned int loadTexture(const char* path);


// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
float aspectRatio = (float)SCR_WIDTH / (float)SCR_HEIGHT; // 1.3333


// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

// lighting
glm::vec3 lightPos(4.0f, 4.0f, 2.0f);

// text
Text inventoryText(SCR_WIDTH, SCR_HEIGHT);

// inventory
Inventory inventory(true);


int main()
{
    // glfw: initialize and configure
    // -------------------------------------------------------------------------------------------

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif


    // glfw window creation
    // -------------------------------------------------------------------------------------------

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "PlateUp-POV", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


    // glad: load all OpenGL function pointers
    // -------------------------------------------------------------------------------------------

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }


    // configure global opengl state
    // -------------------------------------------------------------------------------------------

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    // build and compile our shader zprogram
    // -------------------------------------------------------------------------------------------

    Shader ourShader("shader.vs", "shader.fs");

    Shader lightCubeShader("shader_light.vs", "shader_light.fs");

    Shader crosshairShader("crosshair.vs", "crosshair.fs");

    Shader postItShader("shader_post-it.vs", "shader_post-it.fs");

    Shader textShader("shader_text.vs", "shader_text.fs");
    //Shader shaderSingleColor("stencil_testing.vs", "stencil_testing.fs");

   
    // Floor
    // -------------------------------------------------------------------------------------------

    float planeVertices[] = {
        // positions          // normals           // texture Coords
         5.0f, -0.5f,  5.0f,  0.0f, 1.0f, 0.0f,  4.0f, 0.0f,
        -5.0f, -0.5f,  5.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 1.0f, 0.0f,  0.0f, 4.0f,

         5.0f, -0.5f,  5.0f,  0.0f, 1.0f, 0.0f,  4.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 1.0f, 0.0f,  0.0f, 4.0f,
         5.0f, -0.5f, -5.0f,  0.0f, 1.0f, 0.0f,  4.0f, 4.0f
    };

    unsigned int planeVAO, VBO;
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0); // Position
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))); // Normal
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); // Texture Coords
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    unsigned int floorTexture = loadTexture("resources/images/floor2.jpg");


    // lighting setup
    // -------------------------------------------------------------------------------------------

    float CubeLightVertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };
    // first, configure the cube's VAO (and VBO)

    unsigned int lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(CubeLightVertices), CubeLightVertices, GL_STATIC_DRAW);

    glBindVertexArray(lightCubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // note that we update the lamp's position attribute's stride to reflect the updated buffer data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    ourShader.use();
    ourShader.setInt("texture_diffuse1", 0);
    //ourShader.setInt("texture2", 1);

    // Models
    // -------------------------------------------------------------------------------------------

    Model island("resources/isola/isola_OpenGL.obj");


    //text shader
    // -------------------------------------------------------------------------------------------

    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(SCR_WIDTH), 0.0f, static_cast<float>(SCR_HEIGHT));
    textShader.use();
    glUniformMatrix4fv(glGetUniformLocation(textShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    FT_Library ft = inventoryText.SetFreeType();
    std::string font_name = "resources/fonts/Roboto/Roboto-Bold.ttf";
    std::string font = inventoryText.FindFont(font_name);
    inventoryText.LoadFontAsFace(ft, font);


    // Crosshair Setup
    // -------------------------------------------------------------------------------------------

    float crosshairVertices[] = {
        -0.03f / aspectRatio,  0.0f,
         0.03f / aspectRatio,  0.0f,
          0.0f, -0.03f,
          0.0f,  0.03f
    };

    unsigned int crosshairVAO, crosshairVBO;
    glGenVertexArrays(1, &crosshairVAO);
    glGenBuffers(1, &crosshairVBO);
    glBindVertexArray(crosshairVAO);
    glBindBuffer(GL_ARRAY_BUFFER, crosshairVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(crosshairVertices), crosshairVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    crosshairShader.use();

    //Inventory Setup
    // -------------------------------------------------------------------------------------------

    float postItVertices[] = {
        // positions         // texture coords
        1.0f,  1.0f, 0.0f,   1.0f, 1.0f, // top right
        0.8f,  1.0f, 0.0f,   0.0f, 1.0f, // top left
        0.8f,  0.8f, 0.0f,   0.0f, 0.0f, // bottom left

        1.0f,  1.0f, 0.0f,   1.0f, 1.0f, // top right
        0.8f,  0.8f, 0.0f,   0.0f, 0.0f, // bottom left
        1.0f,  0.8f, 0.0f,   1.0f, 0.0f  // bottom right
    };

    glm::vec3 postItPosition = glm::vec3(650.0f, 450.0f, -0.5f);

    // Post-it VAO/VBO Setup
    unsigned int postItVAO, postItVBO;
    glGenVertexArrays(1, &postItVAO);
    glGenBuffers(1, &postItVBO);
    glBindVertexArray(postItVAO);

    glBindBuffer(GL_ARRAY_BUFFER, postItVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(postItVertices), postItVertices, GL_STATIC_DRAW);

    // Positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Texture Coords
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // Load Post-it Texture
    unsigned int postItTexture = loadTexture("resources/images/post-it.JPG");

    postItShader.use();
    glm::mat4 orthoProjection = glm::ortho(0.0f, static_cast<float>(SCR_WIDTH), 0.0f, static_cast<float>(SCR_HEIGHT));
    postItShader.setMat4("projection", orthoProjection);
    

    /* pos coords
    800.0f, 600.0f, -0.5f,
        500.0f, 300.0f, -0.5f,
        500.0f, 600.0f, -0.5f,
        800.0f, 300.0f, -0.5f*/

        // configure VAO/VBO for texture quads
        // -----------------------------------
    unsigned int textVAO, textVBO;
    glGenVertexArrays(1, &textVAO);
    glGenBuffers(1, &textVBO);
    glBindVertexArray(textVAO);
    glBindBuffer(GL_ARRAY_BUFFER, textVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    textShader.use();


    // RENDER LOOP
    // ---------------------------------------------------------------------------------------------------
    // ---------------------------------------------------------------------------------------------------

    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // SELECTION
        bool cubeSelected = false;
        glm::vec3 rayOrigin = camera.Position;
        glm::vec3 rayDirection = camera.Front;

        // Check for intersection with any cube
        //for (const auto& cubePosition : cubePositions) {
        //    if (rayIntersectsCube(rayOrigin, rayDirection, cubePosition, 1.0f)) {
        //        cubeSelected = true;
        //        break; // Exit loop as one cube is selected
        //    }
       // }

        // If left click is pressed and a cube is selected, exit
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && cubeSelected) {
            glfwSetWindowShouldClose(window, true);
        }

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); 

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, floorTexture);

        // set uniforms
        //shaderSingleColor.use();
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = camera.GetViewMatrix();
        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        ourShader.use();
        ourShader.setMat4("view", view);
        ourShader.setMat4("projection", projection);

        ourShader.setVec3("lightPos", lightPos);
        ourShader.setVec3("viewPos", camera.Position);
        ourShader.setVec3("lightColor", glm::vec3(1.0f)); // White light
        ourShader.setVec3("objectColor", glm::vec3(1.0f, 0.5f, 0.31f)); // Optional if using texture



        // draw floor as normal, but don't write the floor to the stencil buffer, we only care about the containers. We set its mask to 0x00 to not write to the stencil buffer.
        glStencilMask(0x00);
        // floor
        glBindVertexArray(planeVAO);

        ourShader.setMat4("model", glm::mat4(1.0f));
        glDrawArrays(GL_TRIANGLES, 0, 6);
        //glBindVertexArray(0);


        // render the loaded model
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, -0.5f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));  // it's a bit too big for our scene, so scale it down
        ourShader.setMat4("model", model);
        island.Draw(ourShader);

        // also draw the lamp object
        lightCubeShader.use();
        lightCubeShader.setMat4("projection", projection);
        lightCubeShader.setMat4("view", view);
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
        lightCubeShader.setMat4("model", model);

        glBindVertexArray(lightCubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

       
        // After rendering cubes, render the crosshair
        crosshairShader.use();
        glBindVertexArray(crosshairVAO);
        glDrawArrays(GL_LINES, 0, 4);     


        // Draw the inventory

        glDrawArrays(GL_LINES, 0, 4);

        if (inventory.GetState())
        {
            // Render Post-it Fixed to Top-Right Corner
            glEnable(GL_DEPTH_TEST); // Enable depth testing for Post-it rendering
            postItShader.use();
            glBindVertexArray(postItVAO);

            // Bind Post-it Texture
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, postItTexture);
            
            // Model Matrix for Positioning in Top-Right Corner
            model = glm::mat4(1.0f);
            model = glm::translate(model, postItPosition);
            model = glm::scale(model, glm::vec3(200.0f, 150.0f, 1.0f));
            postItShader.setMat4("model", model);
            postItShader.setMat4("projection", projection);

            glDrawArrays(GL_TRIANGLES, 0, 6);
            glBindVertexArray(0);
            //glDisable(GL_DEPTH_TEST); // Disable depth testing for transparent text

            // Enable blending for text rendering
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            textShader.use(); // Ensure text shader is active

            inventoryText.RenderText(textShader, "Inventario", 600.0f, 560.0f, 0.75f, glm::vec3(0.3, 0.7f, 0.9f), textVAO, textVBO);
            inventoryText.RenderText(textShader, "Pomodori ", 610.0f, 530.0f, 0.5f, glm::vec3(0.3, 0.7f, 0.9f), textVAO, textVBO);
            std::string pom = std::to_string(inventory.GetPomodori());
            inventoryText.RenderText(textShader, pom, 740.0f, 530.0f, 0.5f, glm::vec3(0.3, 0.7f, 0.9f), textVAO, textVBO);
            inventoryText.RenderText(textShader, "Insalata ", 610.0f, 508.0f, 0.5f, glm::vec3(0.3, 0.7f, 0.9f), textVAO, textVBO);
            std::string ins = std::to_string(inventory.GetInsalata());
            inventoryText.RenderText(textShader, ins, 740.0f, 508.0f, 0.5f, glm::vec3(0.3, 0.7f, 0.9f), textVAO, textVBO);
            inventoryText.RenderText(textShader, "Pane ", 610.0f, 486.0f, 0.5f, glm::vec3(0.3, 0.7f, 0.9f), textVAO, textVBO);
            std::string pan = std::to_string(inventory.GetPane());
            inventoryText.RenderText(textShader, pan, 740.0f, 486.0f, 0.5f, glm::vec3(0.3, 0.7f, 0.9f), textVAO, textVBO);
            inventoryText.RenderText(textShader, "Hamburger ", 610.0f, 464.0f, 0.5f, glm::vec3(0.3, 0.7f, 0.9f), textVAO, textVBO);
            std::string ham = std::to_string(inventory.GetHamburger());
            inventoryText.RenderText(textShader, ham, 740.0f, 464.0f, 0.5f, glm::vec3(0.3, 0.7f, 0.9f), textVAO, textVBO);

            // Disable blending after text rendering
            glDisable(GL_BLEND);
        }




        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &planeVAO);
    glDeleteVertexArrays(1, &lightCubeVAO);
    glDeleteBuffers(1, &VBO);

    // Add cleanup for crosshair VAO/VBO before terminating GLFW
    glDeleteVertexArrays(1, &crosshairVAO);
    glDeleteBuffers(1, &crosshairVBO);

    // Add cleanup for text VAO/VBO before terminating GLFW
    glDeleteVertexArrays(1, &textVAO);
    glDeleteBuffers(1, &textVBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
    aspectRatio = (float)width / (float)height;
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
    }
    
    stbi_image_free(data);

    return textureID;
}