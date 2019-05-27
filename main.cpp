//internal includes
#include "stb_image.h"
#include "ds.cpp"
#include "common.h"
#include "ShaderProgram.h"
#include "Camera.h"
#include "Model.h"
#include <SOIL/SOIL.h>
// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

//External dependencies
#define GLFW_DLL
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <random>

#define cons 0.9921875 //тк от 129 вершин я перешел к 128, 
  //в некоторых местах нужно будет скорректировать значения.

static const GLsizei WIDTH = 1920, HEIGHT = 1080; //размеры окна
static int filling = 0;
static bool keys[1024]; //массив состояний кнопок - нажата/не нажата
static GLfloat lastX = 400, lastY = 300; //исходное положение мыши
static bool firstMouse = true;
static bool g_captureMouse         = true;  // Мышка захвачена нашим приложением или нет?
static bool g_capturedMouseJustNow = false;
static int SURFACE_SIZE = 40;
void check_fps();


GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

int normalstate = 0;
bool NormalDraw = 0;
bool FogState = 0;
bool waterDraw = 0;
float opacity = 0.9f;
Camera camera(float3(0.0f, 5.0f, 0.0f));

int xxxx = 0;
int zzzz = 0;


//функция для обработки нажатий на кнопки клавиатуры
void OnKeyboardPressed(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	//std::cout << key << std::endl;
	switch (key)
	{
  case GLFW_KEY_ESCAPE: //на Esc выходим из программы
    if (action == GLFW_PRESS)
      glfwSetWindowShouldClose(window, GL_TRUE);
    break;
 	case GLFW_KEY_SPACE: //на пробел переключение в каркасный режим и обратно
		if (action == GLFW_PRESS)
		{
			if (filling == 0)
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				filling = 1;
			}
			else
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				filling = 0;
			}
		}
		break;
  case GLFW_KEY_1:
    normalstate = 0;
    break;
  case GLFW_KEY_F:
    if(action == GLFW_PRESS){
      FogState = !FogState;
    }
  case GLFW_KEY_K:
      if(opacity > 0.2f){
        opacity -= 0.01;
        std::cout << "Water opacity " << opacity << "\n";
      }
      break;
  case GLFW_KEY_L:
      if(opacity < 1.0f){
        opacity += 0.01;
        std::cout <<"Water opacity " << opacity <<"\n";
      }
    break;
  case GLFW_KEY_N:
    if(action == GLFW_PRESS){
      NormalDraw = !NormalDraw;
    }
    break;
  case GLFW_KEY_V:
    if(action == GLFW_PRESS){
      waterDraw = !waterDraw;
    }
    break;
  case GLFW_KEY_2:
    normalstate = 1;
    break;
	default:
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}
}

//функция для обработки клавиш мыши
void OnMouseButtonClicked(GLFWwindow* window, int button, int action, int mods)
{
  if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
    g_captureMouse = !g_captureMouse;


  if (g_captureMouse)
  {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    g_capturedMouseJustNow = true;
  }
  else
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

}

//функция для обработки перемещения мыши
void OnMouseMove(GLFWwindow* window, double xpos, double ypos)
{
  if (firstMouse)
  {
    lastX = float(xpos);
    lastY = float(ypos);
    firstMouse = false;
  }

  GLfloat xoffset = float(xpos) - lastX;
  GLfloat yoffset = lastY - float(ypos);  

  lastX = float(xpos);
  lastY = float(ypos);

  if (g_captureMouse)
    camera.ProcessMouseMove(xoffset, yoffset);
}


void OnMouseScroll(GLFWwindow* window, double xoffset, double yoffset)
{
  camera.ProcessMouseScroll(GLfloat(yoffset));
}

void doCameraMovement(Camera &camera, GLfloat deltaTime)
{
  if (keys[GLFW_KEY_W])
    camera.ProcessKeyboard(FORWARD, deltaTime);
  if (keys[GLFW_KEY_A])
    camera.ProcessKeyboard(LEFT, deltaTime);
  if (keys[GLFW_KEY_S])
    camera.ProcessKeyboard(BACKWARD, deltaTime);
  if (keys[GLFW_KEY_D])
    camera.ProcessKeyboard(RIGHT, deltaTime);
  if(camera.pos.x > SURFACE_SIZE / 2 ) 
    camera.pos.x -= cons * SURFACE_SIZE;
  if(camera.pos.x < -(SURFACE_SIZE / 2)) 
    camera.pos.x += cons * SURFACE_SIZE;
  if(camera.pos.z > SURFACE_SIZE / 2) 
    camera.pos.z -= cons * SURFACE_SIZE;
  if(camera.pos.z < -(SURFACE_SIZE / 2))
    camera.pos.z += cons * SURFACE_SIZE;
}


/**
\brief создать triangle strip плоскость и загрузить её в шейдерную программу
\param rows - число строк
\param cols - число столбцов
\param size - размер плоскости
\param vao - vertex array object, связанный с созданной плоскостью
*/
static int createTriStrip2(int rows, int cols, float size, GLuint &vao)
{
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<double> dist2(1.0, 10.0);

  Matrix<double> height_map = diamond_square(rows, rows) ;//предположим, что нам дают хорошие данные 2^n + 1
    // std::cout   << "LKFJHDCIJNK";





  int numIndices = 2 * cols*(rows - 1) + rows - 1;

  std::vector<GLfloat> vertices_vec; //вектор атрибута координат вершин
  vertices_vec.reserve(rows * cols * 3);

  std::vector<GLfloat> normals_vec; //вектор атрибута нормалей к вершинам
  normals_vec.reserve(rows * cols * 3);

  std::vector<GLfloat> texcoords_vec; //вектор атрибут текстурных координат вершин
  texcoords_vec.reserve(rows * cols * 2);

  std::vector<float3> normals_vec_tmp(rows * cols, float3(0.0f, 0.0f, 0.0f)); //временный вектор нормалей, используемый для расчетов

  std::vector<int3> faces;         //вектор граней (треугольников), каждая грань - три индекса вершин, её составляющих; используется для удобства расчета нормалей
  faces.reserve(numIndices / 3);

  std::vector<GLuint> indices_vec; //вектор индексов вершин для передачи шейдерной программе
  indices_vec.reserve(numIndices);

  for (int z = 0; z < rows; ++z)
  {
    for (int x = 0; x < cols; ++x)
    {
      //вычисляем координаты каждой из вершин 
      float xx = -size / 2 + x*size / cols;
      float zz = -size / 2 + z*size / rows;
      // float yy = -1.0f;
      float r = sqrt(xx*xx + zz*zz);
      // float yy = 15.0f * (r != 0.0f ? sin(r) / r : 1.0f);
      float yy = 0.1 * height_map(z,x);
      // float yy = dist2(mt);

      vertices_vec.push_back(xx);
      vertices_vec.push_back(yy);
      vertices_vec.push_back(zz);

      // texcoords_vec.push_back(x / float(cols - 1)); // вычисляем первую текстурную координату u, для плоскости это просто относительное положение вершины
      // texcoords_vec.push_back(z / float(rows - 1)); // аналогично вычисляем вторую текстурную координату v
    
      texcoords_vec.push_back(x / float(cols - 1)); // вычисляем первую текстурную координату u, для плоскости это просто относительное положение вершины
      texcoords_vec.push_back(z / float(rows - 1)); // аналогично вычисляем вторую текстурную координату v
    }
  }

  //primitive restart - специальный индекс, который обозначает конец строки из треугольников в triangle_strip
  //после этого индекса формирование треугольников из массива индексов начнется заново - будут взяты следующие 3 индекса для первого треугольника
  //и далее каждый последующий индекс будет добавлять один новый треугольник пока снова не встретится primitive restart index

  int primRestart = cols * rows;

  for (int x = 0; x < cols - 1; ++x)
  {
    for (int z = 0; z < rows - 1; ++z)
    {
      int offset = x*cols + z;

      //каждую итерацию добавляем по два треугольника, которые вместе формируют четырехугольник
      if (z == 0) //если мы в начале строки треугольников, нам нужны первые четыре индекса
      {
        indices_vec.push_back(offset + 0);
        indices_vec.push_back(offset + rows);
        indices_vec.push_back(offset + 1);
        indices_vec.push_back(offset + rows + 1);
      }
      else // иначе нам достаточно двух индексов, чтобы добавить два треугольника
      {
        indices_vec.push_back(offset + 1);
        indices_vec.push_back(offset + rows + 1);

        if (z == rows - 2) indices_vec.push_back(primRestart); // если мы дошли до конца строки, вставляем primRestart, чтобы обозначить переход на следующую строку
      }
    }
  }

  ///////////////////////
  //формируем вектор граней(треугольников) по 3 индекса на каждый
  int currFace = 1;
  for (int i = 0; i < indices_vec.size() - 2; ++i)
  {
    int3 face;

    int index0 = indices_vec.at(i);
    int index1 = indices_vec.at(i + 1);
    int index2 = indices_vec.at(i + 2);

    if (index0 != primRestart && index1 != primRestart && index2 != primRestart)
    {
      if (currFace % 2 != 0) //если это нечетный треугольник, то индексы и так в правильном порядке обхода - против часовой стрелки
      {
        face.x = indices_vec.at(i);
        face.y = indices_vec.at(i + 1);
        face.z = indices_vec.at(i + 2);

        currFace++;
      }
      else //если треугольник четный, то нужно поменять местами 2-й и 3-й индекс;
      {    //при отрисовке opengl делает это за нас, но при расчете нормалей нам нужно это сделать самостоятельно
        face.x = indices_vec.at(i);
        face.y = indices_vec.at(i + 2);
        face.z = indices_vec.at(i + 1);

        currFace++;
      }
      faces.push_back(face);
    }
  }


  ///////////////////////
  //расчет нормалей
  for (int i = 0; i < faces.size(); ++i)
  {
    //получаем из вектора вершин координаты каждой из вершин одного треугольника
    float3 A(vertices_vec.at(3 * faces.at(i).x + 0), vertices_vec.at(3 * faces.at(i).x + 1), vertices_vec.at(3 * faces.at(i).x + 2));
    float3 B(vertices_vec.at(3 * faces.at(i).y + 0), vertices_vec.at(3 * faces.at(i).y + 1), vertices_vec.at(3 * faces.at(i).y + 2));
    float3 C(vertices_vec.at(3 * faces.at(i).z + 0), vertices_vec.at(3 * faces.at(i).z + 1), vertices_vec.at(3 * faces.at(i).z + 2));

    //получаем векторы для ребер треугольника из каждой из 3-х вершин
    float3 edge1A(normalize(B - A));
    float3 edge2A(normalize(C - A));

    float3 edge1B(normalize(A - B));
    float3 edge2B(normalize(C - B));

    float3 edge1C(normalize(A - C));
    float3 edge2C(normalize(B - C));

    //нормаль к треугольнику - векторное произведение любой пары векторов из одной вершины
    float3 face_normal = cross(edge1A, edge2A);

    //простой подход: нормаль к вершине = средняя по треугольникам, к которым принадлежит вершина
    normals_vec_tmp.at(faces.at(i).x) += face_normal;
    normals_vec_tmp.at(faces.at(i).y) += face_normal;
    normals_vec_tmp.at(faces.at(i).z) += face_normal;
  }

  //нормализуем векторы нормалей и записываем их в вектор из GLFloat, который будет передан в шейдерную программу
  for (int i = 0; i < normals_vec_tmp.size(); ++i)
  {
    float3 N = normalize(normals_vec_tmp.at(i));

    normals_vec.push_back(N.x);
    normals_vec.push_back(N.y);
    normals_vec.push_back(N.z);
  }



  

  GLuint vboVertices, vboIndices, vboNormals, vboTexCoords;

  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vboVertices);
  glGenBuffers(1, &vboIndices);
  glGenBuffers(1, &vboNormals);
  glGenBuffers(1, &vboTexCoords);


  glBindVertexArray(vao); GL_CHECK_ERRORS;
  {

    //передаем в шейдерную программу атрибут координат вершин
    glBindBuffer(GL_ARRAY_BUFFER, vboVertices); GL_CHECK_ERRORS;
    glBufferData(GL_ARRAY_BUFFER, vertices_vec.size() * sizeof(GL_FLOAT), &vertices_vec[0], GL_STATIC_DRAW); GL_CHECK_ERRORS;
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (GLvoid*)0); GL_CHECK_ERRORS;
    glEnableVertexAttribArray(0); GL_CHECK_ERRORS;

    //передаем в шейдерную программу атрибут нормалей
    glBindBuffer(GL_ARRAY_BUFFER, vboNormals); GL_CHECK_ERRORS;
    glBufferData(GL_ARRAY_BUFFER, normals_vec.size() * sizeof(GL_FLOAT), &normals_vec[0], GL_STATIC_DRAW); GL_CHECK_ERRORS;
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (GLvoid*)0); GL_CHECK_ERRORS;
    glEnableVertexAttribArray(1); GL_CHECK_ERRORS;

    //передаем в шейдерную программу атрибут текстурных координат
    glBindBuffer(GL_ARRAY_BUFFER, vboTexCoords); GL_CHECK_ERRORS;
    glBufferData(GL_ARRAY_BUFFER, texcoords_vec.size() * sizeof(GL_FLOAT), &texcoords_vec[0], GL_STATIC_DRAW); GL_CHECK_ERRORS;
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GL_FLOAT), (GLvoid*)0); GL_CHECK_ERRORS;
    glEnableVertexAttribArray(2); GL_CHECK_ERRORS;

    //передаем в шейдерную программу индексы
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndices); GL_CHECK_ERRORS;
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_vec.size() * sizeof(GLuint), &indices_vec[0], GL_STATIC_DRAW); GL_CHECK_ERRORS;

    glEnable(GL_PRIMITIVE_RESTART); GL_CHECK_ERRORS;
    glPrimitiveRestartIndex(primRestart); GL_CHECK_ERRORS;
  }
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glBindVertexArray(0);


  return numIndices;
}
static int createwater(int rows, int cols, float size, int waterLevel,GLuint &vao)
{
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<double> dist2(1.0, 10.0);

  // Matrix<double> height_map = diamond_square(rows, rows) ;//предположим, что нам дают хорошие данные 2^n + 1
    // std::cout   << "LKFJHDCIJNK";





  int numIndices = 2 * cols*(rows - 1) + rows - 1;

  std::vector<GLfloat> vertices_vec; //вектор атрибута координат вершин
  vertices_vec.reserve(rows * cols * 3);

  std::vector<GLfloat> normals_vec; //вектор атрибута нормалей к вершинам
  normals_vec.reserve(rows * cols * 3);

  std::vector<GLfloat> texcoords_vec; //вектор атрибут текстурных координат вершин
  texcoords_vec.reserve(rows * cols * 2);

  std::vector<float3> normals_vec_tmp(rows * cols, float3(0.0f, 0.0f, 0.0f)); //временный вектор нормалей, используемый для расчетов

  std::vector<int3> faces;         //вектор граней (треугольников), каждая грань - три индекса вершин, её составляющих; используется для удобства расчета нормалей
  faces.reserve(numIndices / 3);

  std::vector<GLuint> indices_vec; //вектор индексов вершин для передачи шейдерной программе
  indices_vec.reserve(numIndices);

  for (int z = 0; z < rows; ++z)
  {
    for (int x = 0; x < cols; ++x)
    {
      //вычисляем координаты каждой из вершин 
      float xx = -size / 2 + x*size / cols;
      float zz = -size / 2 + z*size / rows;
      // float yy = -1.0f;
      float r = sqrt(xx*xx + zz*zz);
      // float yy = 15.0f * (r != 0.0f ? sin(r) / r : 1.0f);
      // float yy = 0.1 * height_map(z,x);
      float yy = waterLevel ;
      // float yy = dist2(mt);

      vertices_vec.push_back(xx);
      vertices_vec.push_back(yy);
      vertices_vec.push_back(zz);

      // texcoords_vec.push_back(x / float(cols - 1)); // вычисляем первую текстурную координату u, для плоскости это просто относительное положение вершины
      // texcoords_vec.push_back(z / float(rows - 1)); // аналогично вычисляем вторую текстурную координату v
    
      texcoords_vec.push_back(x / float(cols - 1)); // вычисляем первую текстурную координату u, для плоскости это просто относительное положение вершины
      texcoords_vec.push_back(z / float(rows - 1)); // аналогично вычисляем вторую текстурную координату v
    }
  }

  //primitive restart - специальный индекс, который обозначает конец строки из треугольников в triangle_strip
  //после этого индекса формирование треугольников из массива индексов начнется заново - будут взяты следующие 3 индекса для первого треугольника
  //и далее каждый последующий индекс будет добавлять один новый треугольник пока снова не встретится primitive restart index

  int primRestart = cols * rows;

  for (int x = 0; x < cols - 1; ++x)
  {
    for (int z = 0; z < rows - 1; ++z)
    {
      int offset = x*cols + z;

      //каждую итерацию добавляем по два треугольника, которые вместе формируют четырехугольник
      if (z == 0) //если мы в начале строки треугольников, нам нужны первые четыре индекса
      {
        indices_vec.push_back(offset + 0);
        indices_vec.push_back(offset + rows);
        indices_vec.push_back(offset + 1);
        indices_vec.push_back(offset + rows + 1);
      }
      else // иначе нам достаточно двух индексов, чтобы добавить два треугольника
      {
        indices_vec.push_back(offset + 1);
        indices_vec.push_back(offset + rows + 1);

        if (z == rows - 2) indices_vec.push_back(primRestart); // если мы дошли до конца строки, вставляем primRestart, чтобы обозначить переход на следующую строку
      }
    }
  }

  ///////////////////////
  //формируем вектор граней(треугольников) по 3 индекса на каждый
  int currFace = 1;
  for (int i = 0; i < indices_vec.size() - 2; ++i)
  {
    int3 face;

    int index0 = indices_vec.at(i);
    int index1 = indices_vec.at(i + 1);
    int index2 = indices_vec.at(i + 2);

    if (index0 != primRestart && index1 != primRestart && index2 != primRestart)
    {
      if (currFace % 2 != 0) //если это нечетный треугольник, то индексы и так в правильном порядке обхода - против часовой стрелки
      {
        face.x = indices_vec.at(i);
        face.y = indices_vec.at(i + 1);
        face.z = indices_vec.at(i + 2);

        currFace++;
      }
      else //если треугольник четный, то нужно поменять местами 2-й и 3-й индекс;
      {    //при отрисовке opengl делает это за нас, но при расчете нормалей нам нужно это сделать самостоятельно
        face.x = indices_vec.at(i);
        face.y = indices_vec.at(i + 2);
        face.z = indices_vec.at(i + 1);

        currFace++;
      }
      faces.push_back(face);
    }
  }


  ///////////////////////
  //расчет нормалей
  for (int i = 0; i < faces.size(); ++i)
  {
    //получаем из вектора вершин координаты каждой из вершин одного треугольника
    float3 A(vertices_vec.at(3 * faces.at(i).x + 0), vertices_vec.at(3 * faces.at(i).x + 1), vertices_vec.at(3 * faces.at(i).x + 2));
    float3 B(vertices_vec.at(3 * faces.at(i).y + 0), vertices_vec.at(3 * faces.at(i).y + 1), vertices_vec.at(3 * faces.at(i).y + 2));
    float3 C(vertices_vec.at(3 * faces.at(i).z + 0), vertices_vec.at(3 * faces.at(i).z + 1), vertices_vec.at(3 * faces.at(i).z + 2));

    //получаем векторы для ребер треугольника из каждой из 3-х вершин
    float3 edge1A(normalize(B - A));
    float3 edge2A(normalize(C - A));

    float3 edge1B(normalize(A - B));
    float3 edge2B(normalize(C - B));

    float3 edge1C(normalize(A - C));
    float3 edge2C(normalize(B - C));

    //нормаль к треугольнику - векторное произведение любой пары векторов из одной вершины
    float3 face_normal = cross(edge1A, edge2A);

    //простой подход: нормаль к вершине = средняя по треугольникам, к которым принадлежит вершина
    normals_vec_tmp.at(faces.at(i).x) += face_normal;
    normals_vec_tmp.at(faces.at(i).y) += face_normal;
    normals_vec_tmp.at(faces.at(i).z) += face_normal;
  }

  //нормализуем векторы нормалей и записываем их в вектор из GLFloat, который будет передан в шейдерную программу
  for (int i = 0; i < normals_vec_tmp.size(); ++i)
  {
    float3 N = normalize(normals_vec_tmp.at(i));

    normals_vec.push_back(N.x);
    normals_vec.push_back(N.y);
    normals_vec.push_back(N.z);
  }



  

  GLuint vboVertices, vboIndices, vboNormals, vboTexCoords;

  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vboVertices);
  glGenBuffers(1, &vboIndices);
  glGenBuffers(1, &vboNormals);
  glGenBuffers(1, &vboTexCoords);


  glBindVertexArray(vao); GL_CHECK_ERRORS;
  {

    //передаем в шейдерную программу атрибут координат вершин
    glBindBuffer(GL_ARRAY_BUFFER, vboVertices); GL_CHECK_ERRORS;
    glBufferData(GL_ARRAY_BUFFER, vertices_vec.size() * sizeof(GL_FLOAT), &vertices_vec[0], GL_STATIC_DRAW); GL_CHECK_ERRORS;
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (GLvoid*)0); GL_CHECK_ERRORS;
    glEnableVertexAttribArray(0); GL_CHECK_ERRORS;

    //передаем в шейдерную программу атрибут нормалей
    glBindBuffer(GL_ARRAY_BUFFER, vboNormals); GL_CHECK_ERRORS;
    glBufferData(GL_ARRAY_BUFFER, normals_vec.size() * sizeof(GL_FLOAT), &normals_vec[0], GL_STATIC_DRAW); GL_CHECK_ERRORS;
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (GLvoid*)0); GL_CHECK_ERRORS;
    glEnableVertexAttribArray(1); GL_CHECK_ERRORS;

    //передаем в шейдерную программу атрибут текстурных координат
    glBindBuffer(GL_ARRAY_BUFFER, vboTexCoords); GL_CHECK_ERRORS;
    glBufferData(GL_ARRAY_BUFFER, texcoords_vec.size() * sizeof(GL_FLOAT), &texcoords_vec[0], GL_STATIC_DRAW); GL_CHECK_ERRORS;
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GL_FLOAT), (GLvoid*)0); GL_CHECK_ERRORS;
    glEnableVertexAttribArray(2); GL_CHECK_ERRORS;

    //передаем в шейдерную программу индексы
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIndices); GL_CHECK_ERRORS;
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_vec.size() * sizeof(GLuint), &indices_vec[0], GL_STATIC_DRAW); GL_CHECK_ERRORS;

    glEnable(GL_PRIMITIVE_RESTART); GL_CHECK_ERRORS;
    glPrimitiveRestartIndex(primRestart); GL_CHECK_ERRORS;
  }
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glBindVertexArray(0);


  return numIndices;
}



int initGL()
{
	int res = 0;
/*
	//грузим функции opengl через glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize OpenGL context" << std::endl;
		return -1;
	}
*/
	//выводим в консоль некоторую информацию о драйвере и контексте opengl
	std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

  std::cout << "Controls: "<< std::endl;
  std::cout << "press left mose button to capture/release mouse cursor  "<< std::endl;
  std::cout << "press spacebar to alternate between shaded wireframe and fill display modes" << std::endl;
  std::cout << "press ESC to exit" << std::endl;

	return 0;
}


  unsigned int loadCubemap(vector<std::string> faces)
  {
  unsigned int textureID;
  glGenTextures(1, &textureID);
  glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

  int width, height, nrChannels;
  for(unsigned int i = 0; i < faces.size(); i++)
  {
  unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
  if(data)
  {
  glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB,
  GL_UNSIGNED_BYTE, data);
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
int main(int argc, char** argv)
{
	if(!glfwInit())
    return -1;

	//запрашиваем контекст opengl версии 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); 
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); 
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); 
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE); 


  GLFWwindow*  window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL basic sample", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	
	glfwMakeContextCurrent(window); 

	//регистрируем коллбеки для обработки сообщений от пользователя - клавиатура, мышь..
	glfwSetKeyCallback        (window, OnKeyboardPressed);  
	glfwSetCursorPosCallback  (window, OnMouseMove); 
  glfwSetMouseButtonCallback(window, OnMouseButtonClicked);
	glfwSetScrollCallback     (window, OnMouseScroll);
	glfwSetInputMode          (window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); 

  // Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
    glewExperimental = GL_TRUE;
    // Initialize GLEW to setup the OpenGL Function pointers
    glewInit();
    /*
	if(initGL() != 0) 
		return -1;
	*/
  //Reset any OpenGL errors which could be present for some reason
	GLenum gl_error = glGetError();
	while (gl_error != GL_NO_ERROR)
		gl_error = glGetError();

	//создание шейдерной программы из двух файлов с исходниками шейдеров
	// используется класс-обертка ShaderProgram

  std::unordered_map<GLenum, std::string> shaders;
  shaders[GL_VERTEX_SHADER]   = "vertex.glsl";
  shaders[GL_FRAGMENT_SHADER] = "fragment.glsl";
  ShaderProgram program(shaders); GL_CHECK_ERRORS;

  std::unordered_map<GLenum, std::string> watershaders;
  watershaders[GL_VERTEX_SHADER]   = "vertexwater.glsl";
  watershaders[GL_FRAGMENT_SHADER] = "fragmentwater.glsl";
  ShaderProgram water(watershaders); GL_CHECK_ERRORS;


  std::unordered_map<GLenum, std::string> lightingshaders;
  lightingshaders[GL_VERTEX_SHADER]   = "lighting_vert.glsl";
  lightingshaders[GL_FRAGMENT_SHADER] = "lighting_frag.glsl";
  ShaderProgram nanosuitShader(lightingshaders); GL_CHECK_ERRORS;

  
  std::unordered_map<GLenum, std::string> skyboxshaders;
  skyboxshaders[GL_VERTEX_SHADER]   = "skyvertex.glsl";
  skyboxshaders[GL_FRAGMENT_SHADER] = "skyfrag.glsl";
  ShaderProgram skyboxShader(skyboxshaders); GL_CHECK_ERRORS;

  std::unordered_map<GLenum, std::string> normalshaders;
  normalshaders[GL_VERTEX_SHADER]   = "normal_vertex.glsl";
  normalshaders[GL_FRAGMENT_SHADER] = "normal_fragment.glsl";
  normalshaders[GL_GEOMETRY_SHADER] = "normal_geometry.glsl";
  ShaderProgram normalShader(normalshaders); GL_CHECK_ERRORS;

  std::unordered_map<GLenum, std::string> waternormalshaders;
  waternormalshaders[GL_VERTEX_SHADER]   = "water_normal_vertex.glsl";
  waternormalshaders[GL_FRAGMENT_SHADER] = "water_normal_fragment.glsl";
  waternormalshaders[GL_GEOMETRY_SHADER] = "water_normal_geometry.glsl";
  ShaderProgram waternormalShader(waternormalshaders); GL_CHECK_ERRORS;

////////////////////////
  //загружаем текстуры
  
  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  int width, height;
  unsigned char* image = SOIL_load_image("textures/terra.jpg", &width, &height, 0, SOIL_LOAD_RGB); 
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
  glGenerateMipmap(GL_TEXTURE_2D);
  SOIL_free_image_data(image);
  glBindTexture(GL_TEXTURE_2D, 0);

  GLuint textureVoter;
  glGenTextures(1, &textureVoter);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, textureVoter);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT); 
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  int width2, height2;
  unsigned char* image2 = SOIL_load_image("textures/water2.jpg", &width2, &height2, 0, SOIL_LOAD_RGB); 
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width2, height2, 0, GL_RGB, GL_UNSIGNED_BYTE, image2);
  glGenerateMipmap(GL_TEXTURE_2D);
  SOIL_free_image_data(image2);
  glBindTexture(GL_TEXTURE_2D, 0); 

  GLuint textureVoterPlane;
  glGenTextures(1, &textureVoterPlane);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, textureVoterPlane);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT); 
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
  // Set texture filtering parameters
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  int widthPlane, heightPlane;
  unsigned char* imagePlane = SOIL_load_image("textures/voter2.png", &widthPlane, &heightPlane, 0, SOIL_LOAD_RGBA); 
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width2, height2, 0, GL_RGBA, GL_UNSIGNED_BYTE, image2);
  glGenerateMipmap(GL_TEXTURE_2D);
  SOIL_free_image_data(imagePlane);
  glBindTexture(GL_TEXTURE_2D, 0); 





 
    glm::vec3 pointLightPosition = glm::vec3(0.7f, 0.2f, 2.0f);
  vector<std::string> faces{"textures/skybox/right.jpg", "textures/skybox/left.jpg",
    "textures/skybox/top.jpg", "textures/skybox/bottom.jpg",
    "textures/skybox/back.jpg", "textures/skybox/front.jpg"};
  unsigned int cubemapTexture = loadCubemap(faces);
  float skyboxVertices[] = {
  // positions
      -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f,
      1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f,
      -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f,
      1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
      1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f,
      1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f,
      -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
      -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f,
      1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f};
  unsigned int skyboxVBO, skyboxVAO;
  glGenBuffers(1, &skyboxVBO);
  glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
  glGenVertexArrays(1, &skyboxVAO);
  glBindVertexArray(skyboxVAO);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);


  //Создаем и загружаем геометрию поверхности
  GLuint vaoTriStrip;
  GLuint waterVAO;
  int triStripIndices = createTriStrip2(128, 128, SURFACE_SIZE, vaoTriStrip);
  int waterIndices = createwater(128, 128, SURFACE_SIZE, 2, waterVAO);

  glViewport(0, 0, WIDTH, HEIGHT);  GL_CHECK_ERRORS;
  glEnable(GL_DEPTH_TEST);  GL_CHECK_ERRORS;
  glEnable(GL_BLEND);  GL_CHECK_ERRORS;

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  Model nanosuit("model/nanosuit.obj");
  // std::cout << "Model loaded" << "\n";


      nanosuitShader.StartUseShader();
    // directional light
    nanosuitShader.SetUniform("dirLight.direction", float3(1.0f, 1.0f, 0.0f));
    nanosuitShader.SetUniform("dirLight.ambient", float3(0.2f, 0.2f, 0.2f));
    nanosuitShader.SetUniform("dirLight.diffuse", float3(0.6f, 0.6f, 0.6f));
    nanosuitShader.SetUniform("dirLight.specular", float3(0.5f, 0.5f, 0.5f));
  //цикл обработки сообщений и отрисовки сцены каждый кадр
  while (!glfwWindowShouldClose(window))
  {
    //считаем сколько времени прошло за кадр
    GLfloat currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    glfwPollEvents();
    doCameraMovement(camera, deltaTime);

    //очищаем экран каждый кадр
    glClearColor(0.81f, 0.81f, 0.81f, 1.0f); GL_CHECK_ERRORS;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); GL_CHECK_ERRORS;

		//обновляем матрицы камеры и проекции каждый кадр
    float4x4 view       = camera.GetViewMatrix();
    float4x4 projection = projectionMatrixTransposed(camera.zoom, float(WIDTH) / float(HEIGHT), 0.1f, 1000.0f);

                    //модельная матрица, определяющая положение объекта в мировом пространстве
    float4x4 model; //начинаем с единичной матрицы

    if(!FogState)
    {
      glDepthMask(GL_FALSE);
      skyboxShader.StartUseShader();
      // view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
      view = camera.GetViewMatrix();
      skyboxShader.SetUniform("view", view);
      skyboxShader.SetUniform("projection", projection);
      // skyboxShader.SetUniform("FogState", static_cast<int>(FogState)); GL_CHECK_ERRORS;
      // skybox cube
      glBindVertexArray(skyboxVAO);
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
      glDrawArrays(GL_TRIANGLES, 0, 36);
      glBindVertexArray(0);
      glDepthMask(GL_TRUE);
    }
    
    program.StartUseShader(); GL_CHECK_ERRORS;

    view       = camera.GetViewMatrix();
    projection = projectionMatrixTransposed(camera.zoom, float(WIDTH) / float(HEIGHT), 0.1f, 1000.0f);

      glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(glGetUniformLocation(program.GetProgram(), "ourTexture1"), 0);
      glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, textureVoter);
        glUniform1i(glGetUniformLocation(program.GetProgram(), "textureVoter"), 1);

    float3 llight = float3(1.0f, 1.0f, 1.0f);
    // program.StartUseShader();

    //загружаем uniform-переменные в шейдерную программу (одинаковые для всех параллельно запускаемых копий шейдера)
    program.SetUniform("view",       view);       GL_CHECK_ERRORS;
    program.SetUniform("projection", projection); GL_CHECK_ERRORS;
    program.SetUniform("time", currentFrame); GL_CHECK_ERRORS;
    program.SetUniform("normalized", normalstate); GL_CHECK_ERRORS;
    program.SetUniform("FogState", static_cast<int>(FogState)); GL_CHECK_ERRORS;
      program.SetUniform("waterState", static_cast<int>(waterDraw));GL_CHECK_ERRORS;


    

    //рисуем плоскость
    glBindVertexArray(vaoTriStrip);
    for (int i = 0; i < 9; ++i)
    {
      model = glm::translate(glm::mat4(), glm::vec3((i / 3 - 1) * cons *SURFACE_SIZE  , 0.0f, (i % 3 - 1) * cons *SURFACE_SIZE));
      program.SetUniform("model",      model);
      glDrawElements(GL_TRIANGLE_STRIP, triStripIndices, GL_UNSIGNED_INT, nullptr); GL_CHECK_ERRORS;
    }
    glBindVertexArray(0); GL_CHECK_ERRORS;


    program.StopUseShader();

    if(NormalDraw){
      normalShader.StartUseShader();
      normalShader.SetUniform("view",       view);       GL_CHECK_ERRORS;
      normalShader.SetUniform("projection", projection); GL_CHECK_ERRORS;  
      glBindVertexArray(vaoTriStrip);
      for (int i = 0; i < 9; ++i){
        model = glm::translate(glm::mat4(), glm::vec3((i / 3 - 1) * cons *SURFACE_SIZE  , 0.0f, (i % 3 - 1) * cons *SURFACE_SIZE));
        normalShader.SetUniform("model",      model);
        glDrawElements(GL_TRIANGLE_STRIP, triStripIndices, GL_UNSIGNED_INT, nullptr); GL_CHECK_ERRORS;
      }
      glBindVertexArray(0); GL_CHECK_ERRORS;
      normalShader.StopUseShader();
    }
    if(waterDraw){
      water.StartUseShader();
          glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, textureVoterPlane);
        glUniform1i(glGetUniformLocation(water.GetProgram(), "textureVoter"), 1);
        
    water.SetUniform("cameraPos", camera.pos); GL_CHECK_ERRORS;
    water.SetUniform("skybox", 0); GL_CHECK_ERRORS;
    water.SetUniform("time", currentFrame); GL_CHECK_ERRORS;
      water.SetUniform("model",      model);
      water.SetUniform("view",       view);       GL_CHECK_ERRORS;
      water.SetUniform("projection", projection); GL_CHECK_ERRORS;
       water.SetUniform("FogState", static_cast<int>(FogState)); GL_CHECK_ERRORS;
       water.SetUniform("opacity", static_cast<float>(opacity)); GL_CHECK_ERRORS;
       water.SetUniform("normalized", normalstate); GL_CHECK_ERRORS;
      glBindVertexArray(waterVAO);
      for (int i = 0; i < 9; ++i){
        model = glm::translate(glm::mat4(), glm::vec3((i / 3 - 1) * cons *SURFACE_SIZE  , 0.0f, (i % 3 - 1) * cons *SURFACE_SIZE));
        water.SetUniform("model",      model);
        glDrawElements(GL_TRIANGLE_STRIP, waterIndices, GL_UNSIGNED_INT, nullptr); GL_CHECK_ERRORS;
      }
      water.StopUseShader();
      if(NormalDraw){
      waternormalShader.StartUseShader();
      waternormalShader.SetUniform("view",       view);       GL_CHECK_ERRORS;
      waternormalShader.SetUniform("projection", projection); GL_CHECK_ERRORS;  
      waternormalShader.SetUniform("time", currentFrame); GL_CHECK_ERRORS;
      glBindVertexArray(waterVAO);
      for (int i = 0; i < 9; ++i){
        model = glm::translate(glm::mat4(), glm::vec3((i / 3 - 1) * cons *SURFACE_SIZE  , 0.0f, (i % 3 - 1) * cons *SURFACE_SIZE));
        waternormalShader.SetUniform("model",      model);
        glDrawElements(GL_TRIANGLE_STRIP, waterIndices, GL_UNSIGNED_INT, nullptr); GL_CHECK_ERRORS;
      }
      glBindVertexArray(0); GL_CHECK_ERRORS;
      waternormalShader.StopUseShader();
    }
    }
          nanosuitShader.StartUseShader();
        nanosuitShader.SetUniform("projection", projection);
        nanosuitShader.SetUniform("view", view);
    

        // render the loaded model
        glm::mat4 model2;
        model2 = glm::translate(model2, glm::vec3(2.0f, 10.0f, 3.0f));
        model2 = glm::scale(model2, glm::vec3(0.2f, 0.2f, 0.2f));    
        model2 = glm::translate(model2, glm::vec3(10.0f, 15.0f, 0.0f));
        nanosuitShader.SetUniform("model", model2);
        nanosuit.Draw(nanosuitShader);


		glfwSwapBuffers(window); 
    // check_fps();
	}

	//очищаем vao перед закрытием программы
  glDeleteVertexArrays(1, &vaoTriStrip);
	glDeleteVertexArrays(1, &waterVAO);

	glfwTerminate();
	return 0;
}

void check_fps()
{
    static int nFrame = 0;
    static GLfloat lastTime = glfwGetTime();
    static int lastFrame = 0;
    ++nFrame;
    GLfloat currTime = glfwGetTime();
    if(currTime - lastTime > 1.0f)
    {
        printf("FPS: %f\n", (float(nFrame - lastFrame) / (currTime - lastTime)));
        lastTime = currTime;
        lastFrame = nFrame;
    }
}
