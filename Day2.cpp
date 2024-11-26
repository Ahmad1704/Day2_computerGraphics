#include <iostream>
#include <gl/glew.h>
#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#ifndef GLM_MESSAGES
#define GLM_MESSAGES
#endif
#define GLM_ENABLE_EXPERIMENTAL
#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtx\transform.hpp>



using namespace std;
using namespace glm;

enum DrawingMode
{
    Points,
    Lines,
    FilledTriangle
};

struct vertex
{
    vec3 position;
    vec3 color;
    vertex(vec3 _position, vec3 _color) : position{ _position }, color{ _color }
    {
    }
    vertex(vec3 _position) : position{ _position }, color{ _position }
    {
    }
};

GLuint InitShader(const char* vertex_shader_file_name, const char* fragment_shader_file_name);

const GLint WIDTH = 600, HEIGHT = 600;
GLuint VBO_Cube, IBO, BasiceprogramId;
DrawingMode Current_DrawingMode = DrawingMode::FilledTriangle;

// transformation
GLuint modelMatLoc, viewMatLoc, projMatLoc;

vertex cube_core_vertices[] = {
    {vec3(-0.5, 0.5, 0.5), vec3(1, 0, 0)},
    {vec3(-0.5, -0.5, 0.5), vec3(0, 1, 0)},
    {vec3(0.5, -0.5, 0.5), vec3(1, 0, 1)},
    {vec3(0.5, 0.5, 0.5), vec3(0, 0, 1)},
    {vec3(0.5, 0.5, -0.5), vec3(1, 1, 0)},
    {vec3(0.5, -0.5, -0.5), vec3(0, 1, 1)},
    {vec3(-0.5, -0.5, -0.5), vec3(1, 1, 1)},
    {vec3(-0.5, 0.5, -0.5), vec3(0, 0, 0)}
};

void CreateCube()
{
    int vertices_Indeces[] = {
        //front
        0, 1, 2,
        0, 2, 3,
        //Right
        3, 2, 5,
        3, 5, 4,
        //Back
        4, 5, 6,
        4, 6, 7,
        //Left
        7, 6, 1,
        7, 1, 0,
        //Top
        7, 0, 3,
        7, 3, 4,
        //Bottom
        2, 1, 6,
        2, 6, 5
    };

    // create VBO
    glGenBuffers(1, &VBO_Cube);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_Cube);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_core_vertices), cube_core_vertices, GL_STATIC_DRAW);

    // Index Buffer
    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(vertices_Indeces), vertices_Indeces, GL_STATIC_DRAW);

    // shader
    glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(vertex), 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(vertex), (char*)(sizeof(vec3)));
    glEnableVertexAttribArray(1);
}

void BindCube()
{
    glBindBuffer(GL_ARRAY_BUFFER, VBO_Cube);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

    glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(vertex), 0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(vertex), (char*)(sizeof(vec3)));
    glEnableVertexAttribArray(1);
}

void CompileShader(const char* vertex_shader_file_name, const char* fragment_shader_file_name, GLuint& programId)
{
    programId = InitShader(vertex_shader_file_name, fragment_shader_file_name);
    glUseProgram(programId);
}

int Init()
{
    GLenum err = glewInit();
    if (err != GLEW_OK)
    {
        cout << "Error" << endl;
        getchar();
        return 1;
    }
    else
    {
        if (GLEW_VERSION_3_0)
            cout << "Driver support OpenGL 3.0\nDetails:\n";
    }
    cout << "\tUsing glew " << glewGetString(GLEW_VERSION) << endl;
    cout << "\tVendor: " << glGetString(GL_VENDOR) << endl;
    cout << "\tRenderer: " << glGetString(GL_RENDERER) << endl;
    cout << "\tVersion: " << glGetString(GL_VERSION) << endl;
    cout << "\tGLSL:" << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;

    CompileShader("VS.glsl", "FS.glsl", BasiceprogramId);
    CreateCube();

    modelMatLoc = glGetUniformLocation(BasiceprogramId, "modelMat");
    viewMatLoc = glGetUniformLocation(BasiceprogramId, "viewMat");
    projMatLoc = glGetUniformLocation(BasiceprogramId, "projMat");

    glm::mat4 viewMat = glm::lookAt(glm::vec3(0, 0, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    glUniformMatrix4fv(viewMatLoc, 1, GL_FALSE, glm::value_ptr(viewMat));

    glm::mat4 projMat = glm::perspectiveFov(1.f, (float)WIDTH, (float)HEIGHT, .1f, 100.0f);
    glUniformMatrix4fv(projMatLoc,1, GL_FALSE, glm::value_ptr(projMat));

    glClearColor(0, 0.5, 0.5, 1);
    glEnable(GL_DEPTH_TEST);

    return 0;
}

float theta = 0;
float posX = 0;

void Update()
{
    // Increase theta for rotation
    theta += 0.1f;              // Adjust this to control rotation speed

    // Move across the screen
    posX += 0.001f;

    // Reset position when it moves off screen
    if (posX > 1.0f)
    {
        posX = -1.0f;
        theta = 0;
    }
}

void Render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set drawing mode based on user input (Points, Lines, or FilledTriangles)
    switch (Current_DrawingMode)
    {
    case Points:
        glPointSize(10);
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
        break;
    case Lines:
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        break;
    case FilledTriangle:
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        break;
    default:
        break;
    }

    // Bind the cube for all body parts
    BindCube();

    mat4 ModelMat;  // Model matrix for the entire body


    if (theta <= 90)
    {
        // Apply rotation only
        ModelMat = glm::rotate(theta, glm::vec3(0.0f, 1.0f, 0.0f));
    }
    else
    {
        // Apply translation after rotation once theta exceeds 90 degrees
        ModelMat = glm::rotate(90.0f, glm::vec3(0.0f, 1.0f, 0.0f)) *
            glm::translate(glm::vec3(0.0f, 0.0f, posX));  // Translate the body across the screen
    }

    // Draw the torso (cube for the body)
    mat4 torsoModelMat = ModelMat * glm::translate(vec3(0.0f, 0.0f, 0.0f)) *
        glm::scale(vec3(1.0f, 1.0f, 1.0f));  // Position and scale torso
    glUniformMatrix4fv(modelMatLoc, 1, GL_FALSE, glm::value_ptr(torsoModelMat));
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, NULL);

    // Draw the head (cube for the head)
    mat4 headModelMat = ModelMat * glm::translate(vec3(0.0f, 0.75f, 0.0f)) *
        glm::scale(vec3(0.5f, 0.5f, 0.5f));  // Position and scale head
    glUniformMatrix4fv(modelMatLoc, 1, GL_FALSE, glm::value_ptr(headModelMat));
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, NULL);

    // Draw the left arm (cube for the left arm)
    mat4 leftArmModelMat = ModelMat * glm::translate(vec3(-0.8f, 0.0f, 0.0f)) *
        glm::scale(vec3(0.3f, 1.0f, 0.3f));  // Position and scale left arm
    glUniformMatrix4fv(modelMatLoc, 1, GL_FALSE, glm::value_ptr(leftArmModelMat));
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, NULL);

    // Draw the right arm (cube for the right arm)
    mat4 rightArmModelMat = ModelMat * glm::translate(vec3(0.8f, 0.0f, 0.0f)) *
        glm::scale(vec3(0.3f, 1.0f, 0.3f));  // Position and scale right arm
    glUniformMatrix4fv(modelMatLoc, 1, GL_FALSE, glm::value_ptr(rightArmModelMat));
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, NULL);

    // Draw the left leg (cube for the left leg)
    mat4 leftLegModelMat = ModelMat * glm::translate(vec3(-0.3f, -0.7f, 0.0f)) *
        glm::scale(vec3(0.3f, 1.0f, 0.3f));  // Position and scale left leg
    glUniformMatrix4fv(modelMatLoc, 1, GL_FALSE, glm::value_ptr(leftLegModelMat));
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, NULL);

    // Draw the right leg (cube for the right leg)
    mat4 rightLegModelMat = ModelMat * glm::translate(vec3(0.3f, -0.7f, 0.0f)) *
        glm::scale(vec3(0.3f, 1.0f, 0.3f));  // Position and scale right leg
    glUniformMatrix4fv(modelMatLoc, 1, GL_FALSE, glm::value_ptr(rightLegModelMat));
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, NULL);
}






int main()
{
    sf::ContextSettings context;
    context.depthBits = 24;
    sf::Window window(sf::VideoMode(WIDTH, HEIGHT), "SFML works!", sf::Style::Close, context);

    if (Init()) return 1;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            switch (event.type)
            {
            case sf::Event::Closed:
                window.close();
                break;
            case sf::Event::KeyPressed:
                if (event.key.code == sf::Keyboard::Num1)
                {
                    Current_DrawingMode = DrawingMode::Points;
                }
                if (event.key.code == sf::Keyboard::Num2)
                {
                    Current_DrawingMode = DrawingMode::Lines;
                }
                if (event.key.code == sf::Keyboard::Num3)
                {
                    Current_DrawingMode = DrawingMode::FilledTriangle;
                }
                break;
            }
        }

        Update();
        Render();

        window.display();
    }
    return 0;
}
