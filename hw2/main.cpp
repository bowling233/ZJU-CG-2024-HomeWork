#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <iostream>

const int WIDTH = 450;
const int HEIGHT = 300;

// scale 45,30 to 2,2
glm::mat3 m = glm::mat3(2.0f / 45, 0.0f, 0.0f,
                        0.0f, 2.0f / 30, 0.0f,
                        0.0f, 0.0f, 1.0f);

glm::vec3 stripes[] = {
    glm::vec3(-1.0f, 1.0f, 0.0f),
    glm::vec3(-1.0f, 0.6f, 0.0f),
    glm::vec3(1.0f, 0.6f, 0.0f),
    glm::vec3(1.0f, 1.0f, 0.0f),

    glm::vec3(-1.0f, 0.2f, 0.0f),
    glm::vec3(-1.0f, -0.2f, 0.0f),
    glm::vec3(1.0f, -0.2f, 0.0f),
    glm::vec3(1.0f, 0.2f, 0.0f),

    glm::vec3(-1.0f, -0.6f, 0.0f),
    glm::vec3(-1.0f, -1.0f, 0.0f),
    glm::vec3(1.0f, -1.0f, 0.0f),
    glm::vec3(1.0f, -0.6f, 0.0f),

    glm::vec3(-1.0f, 0.6f, 0.0f),
    glm::vec3(-1.0f, 0.2f, 0.0f),
    glm::vec3(1.0f, 0.2f, 0.0f),
    glm::vec3(1.0f, 0.6f, 0.0f),

    glm::vec3(-1.0f, -0.2f, 0.0f),
    glm::vec3(-1.0f, -0.6f, 0.0f),
    glm::vec3(1.0f, -0.6f, 0.0f),
    glm::vec3(1.0f, -0.2f, 0.0f)};

glm::vec3 triangle[] = {
    m * glm::vec3(-22.5f, 15.0f, 0.0f),
    m *glm::vec3(-22.5f + 15.0f * sqrt(3.0f), 0.0f, 0.0f),
    m *glm::vec3(-22.5f, -15.0f, 0.0f)};

GLfloat r = 6.0f;
GLfloat x = r / (1.0f + 1.0f / (tan(glm::pi<float>() * 3.0f / 10.0f) * tan(glm::pi<float>() / 10.0f)));
GLfloat y = x / sin(glm::pi<float>() * 3.0f / 10.0f);
GLfloat z = (x + y) / tan(glm::pi<float>() / 5.0f);
GLfloat d = 22.5f - sqrt(3.0f) * 30.0f / 6.0f;

glm::mat2 rotate(float angle)
{
        return glm::mat2(cos(angle), -sin(angle), sin(angle), cos(angle));
}

glm::vec3 left(glm::vec3 v, float d)
{
        return glm::vec3(v.x - d, v.y, v.z);
}

glm::vec3 star[] = {
    m * left(glm::vec3(0.0f, -y, 0.0f), d),
    m *left(glm::vec3(-z, x, 0.0f), d),
    m *left(glm::vec3(z, x, 0.0f), d),
    m *left(glm::vec3(rotate(glm::pi<float>() * 2 / 5) * glm::vec2(0.0f, -y), 0.0f), d),
    m *left(glm::vec3(rotate(glm::pi<float>() * 2 / 5) * glm::vec2(-z, x), 0.0f), d),
    m *left(glm::vec3(rotate(glm::pi<float>() * 2 / 5) * glm::vec2(z, x), 0.0f), d),
    m *left(glm::vec3(rotate(glm::pi<float>() * -2 / 5) * glm::vec2(0.0f, -y), 0.0f), d),
    m *left(glm::vec3(rotate(glm::pi<float>() * -2 / 5) * glm::vec2(-z, x), 0.0f), d),
    m *left(glm::vec3(rotate(glm::pi<float>() * -2 / 5) * glm::vec2(z, x), 0.0f), d)};

std::ostream &operator<<(std::ostream &os, const glm::vec3 &v)
{
        os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
        return os;
}

void initOpenGL()
{
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        std::cout << glm::vec3(0.0f, -y, 0.0f) << std::endl;
        std::cout << glm::vec3(-z, x, 0.0f) << std::endl;
        std::cout << glm::vec3(z, x, 0.0f) << std::endl;
        std::cout << star[0] << std::endl;
        std::cout << star[1] << std::endl;
        std::cout << star[2] << std::endl;
}

void display()
{
        glClear(GL_COLOR_BUFFER_BIT);

        glEnableClientState(GL_VERTEX_ARRAY);

        glColor3f(0.78f, 0.0f, 0.08f);
        glVertexPointer(3, GL_FLOAT, 0, stripes);
        glDrawArrays(GL_QUADS, 0, 12);

        glColor3f(1.0f, 1.0f, 1.0f);
        glDrawArrays(GL_QUADS, 12, 8);

        glColor3f(0.0f, 0.22f, 0.66f);
        glVertexPointer(3, GL_FLOAT, 0, triangle);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glColor3f(1.0f, 1.0f, 1.0f);
        glVertexPointer(3, GL_FLOAT, 0, star);
        glDrawArrays(GL_TRIANGLES, 0, 9);

        glDisableClientState(GL_VERTEX_ARRAY);

        glFlush();
}

int main()
{
        if (!glfwInit())
        {
                return -1;
        }

        GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "Puerto Rico Flag", NULL, NULL);
        if (!window)
        {
                glfwTerminate();
                return -1;
        }

        glfwMakeContextCurrent(window);
        glewInit();

        initOpenGL();

        while (!glfwWindowShouldClose(window))
        {
                display();
                glfwSwapBuffers(window);
                glfwPollEvents();
        }

        glfwTerminate();
        return 0;
}
