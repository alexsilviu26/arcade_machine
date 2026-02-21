#include "lab_m1/Tema1/object2D.h"

#include <vector>
#include <cmath>
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include "core/engine.h"
#include "utils/gl_utils.h"

//functie ce creeaza un patrat
Mesh* object2D::CreateBlock(
    const std::string& name,
    glm::vec3 leftBottomCorner,
    float length,
    glm::vec3 color,
    bool fill)
{
    glm::vec3 corner = leftBottomCorner;

    std::vector<VertexFormat> vertices =
    {
        VertexFormat(corner, color),
        VertexFormat(corner + glm::vec3(length, 0, 0), color),
        VertexFormat(corner + glm::vec3(length, length, 0), color),
        VertexFormat(corner + glm::vec3(0, length, 0), color)
    };

    Mesh* square = new Mesh(name);
    std::vector<unsigned int> indices = { 0, 1, 2, 3 };

    if (!fill) {
        square->SetDrawMode(GL_LINE_LOOP);
    }
    else {
        
        indices.push_back(0);
        indices.push_back(2);
    }

    square->InitFromData(vertices, indices);
    return square;
}


//funcite ce creeaza un tun;
Mesh* object2D::CreateGun(
    const std::string& name,
    glm::vec3 leftBottomCorner,
    float length,
    glm::vec3 baseColor,
    glm::vec3 arcColor,
    glm::vec3 barrelColor,
    bool fill)
{
    glm::vec3 corner = leftBottomCorner;
    float L = length;

    std::vector<VertexFormat> vertices = 
    {
		//baza patrat 1x1
        VertexFormat(corner, baseColor),
        VertexFormat(corner + glm::vec3(L, 0, 0), baseColor),
        VertexFormat(corner + glm::vec3(L, L, 0), baseColor),
        VertexFormat(corner + glm::vec3(0, L, 0), baseColor),
    };
    std::vector<unsigned int> indices = { 0, 1,  2, 0, 2, 3 };

	//semicercul de deasupra patratului
    glm::vec3 arcCenter = corner + glm::vec3(L / 2, L, 0);
    float arcRadius = L / 2; // raza e L/2, ca sa se potriveasca pe patrat
    int numSegments = 20;

    // Varfurile incepand de la 4
    int centerIndex = vertices.size();
    vertices.push_back(VertexFormat(arcCenter, arcColor));

	//adaugam varfurile de pe arc
    int firstArcIndex = vertices.size(); // Va fi 5
    for (int i = 0; i <= numSegments; ++i) {
        float angle = (float)i * (float)M_PI / numSegments;
        glm::vec3 arcPos = arcCenter + glm::vec3(cos(angle) * arcRadius, sin(angle) * arcRadius, 0);
        vertices.push_back(VertexFormat(arcPos, arcColor));
    }

    // adauga indecsii pentru triunghiurile semicercului
    for (int i = 0; i < numSegments; ++i) {
        indices.push_back(centerIndex);
        indices.push_back(firstArcIndex + i);
        indices.push_back(firstArcIndex + i + 1);
    }

    // teava este deasupra bazei 1x1
    // incepe de la y=L, suprapunandu-se cu semicercul
    int offset = vertices.size();
    vertices.push_back(VertexFormat(corner + glm::vec3(0, L, 0), barrelColor));
    vertices.push_back(VertexFormat(corner + glm::vec3(L, L, 0), barrelColor));
    vertices.push_back(VertexFormat(corner + glm::vec3(L, L + 2 * L, 0), barrelColor));
    vertices.push_back(VertexFormat(corner + glm::vec3(0, L + 2 * L, 0), barrelColor));

    indices.push_back(offset + 0);
    indices.push_back(offset + 1);
    indices.push_back(offset + 2);
    indices.push_back(offset + 0);
    indices.push_back(offset + 2);
    indices.push_back(offset + 3);

    // creare mesh
    Mesh* gun = new Mesh(name);

    if (!fill) {
        gun->SetDrawMode(GL_LINE_LOOP);
    }

    gun->InitFromData(vertices, indices);
    return gun;
}

//functie ce creeaza un bumper  
Mesh* object2D::CreateBumper(
    const std::string& name,
    glm::vec3 leftBottomCorner,
    float length,
    glm::vec3 baseColor,
    glm::vec3 arcColor,
    bool fill)
{
    glm::vec3 corner = leftBottomCorner;
    float L = length;

    std::vector<VertexFormat> vertices =
    {
        // baza patrat 
        VertexFormat(corner, baseColor),
        VertexFormat(corner + glm::vec3(L, 0, 0), baseColor),
        VertexFormat(corner + glm::vec3(L, L, 0), baseColor),
        VertexFormat(corner + glm::vec3(0, L, 0), baseColor),
    };
    std::vector<unsigned int> indices = { 0, 1,  2, 0, 2, 3 };

    // jumatate de oval
    // centrul ovalului este la mijlocul laturii de sus a patratului
    glm::vec3 arcCenter = corner + glm::vec3(L / 2, L, 0);

    // folosim raze diferite pentru X si Y
    float radiusX = 1.5f * L;
    float radiusY = 1.0f * L;

    int numSegments = 20;

    // adauga varful central (varful 4)
    int centerIndex = vertices.size();
    vertices.push_back(VertexFormat(arcCenter, arcColor));

    // adauga varfurile de pe arc
    int firstArcIndex = vertices.size();
    for (int i = 0; i <= numSegments; ++i) {
        float angle = (float)i * (float)M_PI / numSegments;
        glm::vec3 arcPos = arcCenter + glm::vec3(cos(angle) * radiusX, sin(angle) * radiusY, 0);
        vertices.push_back(VertexFormat(arcPos, arcColor));
    }

    // Adauga indecsii pentru triunghiurile ovalului
    for (int i = 0; i < numSegments; ++i) {
        indices.push_back(centerIndex);
        indices.push_back(firstArcIndex + i);
        indices.push_back(firstArcIndex + i + 1);
    }

    // --- Creare Mesh ---
    Mesh* bumper = new Mesh(name);

    if (!fill) {
        bumper->SetDrawMode(GL_LINE_LOOP);
    }
  
    bumper->InitFromData(vertices, indices);
    return bumper;
}

//functie ce creeaza un motor
Mesh* object2D::CreateEngine(
    const std::string& name,
    glm::vec3 leftBottomCorner,
    float length,
    glm::vec3 color,
    bool fill)
{
    glm::vec3 corner = leftBottomCorner;
    glm::vec3 yellow(1.0f, 1.0f, 0.0f);
	glm::vec3 orange(1.0f, 0.7f, 0.0f);

    std::vector<VertexFormat> vertices =
    {
        VertexFormat(corner, color),
        VertexFormat(corner + glm::vec3(length, 0, 0), color),
        VertexFormat(corner + glm::vec3(length, length, 0), color),
        VertexFormat(corner + glm::vec3(0, length, 0), color),
        VertexFormat(corner + glm::vec3(length / 6.0f ,0, 0), yellow),
		VertexFormat(corner + glm::vec3(5 * length / 6.0f, 0, 0), yellow),
		VertexFormat(corner + glm::vec3(length / 6, -length / 2, 0), yellow),
		VertexFormat(corner + glm::vec3(5 * length / 6.0f, -length / 2, 0), yellow),
		VertexFormat(corner + glm::vec3(length / 2.0f, -length / 2, 0), orange),
        VertexFormat(corner + glm::vec3(length / 6.0f ,0, 0), orange),
        VertexFormat(corner + glm::vec3(5 * length / 6.0f, 0, 0), orange),

    };

    Mesh* square = new Mesh(name);
    std::vector<unsigned int> indices = { 0, 1, 2, 0, 2, 3, 9, 8, 10, 4, 6, 5, 4, 7, 5 };



    if (!fill) {
        square->SetDrawMode(GL_LINE_LOOP);
    }
    else {
        indices.push_back(0);
        indices.push_back(2);
    }

    square->InitFromData(vertices, indices);
    return square;
}

//functie ce creeaza butonul de start
//doua triunghiuri suprapuse
Mesh* object2D::CreateButton(
    const std::string& name,
    glm::vec3 leftBottomCorner,
    float length,
    glm::vec3 color,
    bool fill)
{
    glm::vec3 corner = leftBottomCorner;
    glm::vec3 yellow(1.0f, 1.0f, 0.0f);
    glm::vec3 orange(1.0f, 0.7f, 0.0f);

    std::vector<VertexFormat> vertices =
    {
        VertexFormat(corner, color),
        VertexFormat(corner + glm::vec3(length, 0, 0), color),
        VertexFormat(corner + glm::vec3(length, length, 0), color),
        VertexFormat(corner + glm::vec3(0, length, 0), color),

    };

    Mesh* square = new Mesh(name);
    std::vector<unsigned int> indices = { 0, 1, 3, 0, 3, 2 };



    if (!fill) {
        square->SetDrawMode(GL_LINE_LOOP);
    }
    else {
        indices.push_back(0);
        indices.push_back(2);
    }

    square->InitFromData(vertices, indices);
    return square;
}

//functie ce creeaza o sageata
//pentru start
Mesh* object2D::CreateArrow(
    const std::string& name,
    glm::vec3 leftBottomCorner,
    float length,
    glm::vec3 color,
    bool fill
)
{
    glm::vec3 corner = leftBottomCorner;
    glm::vec3 yellow(1.0f, 1.0f, 0.0f);
    glm::vec3 orange(1.0f, 0.7f, 0.0f);

    std::vector<VertexFormat> vertices =
    {
        VertexFormat(corner, color),
        VertexFormat(corner + glm::vec3(length, 0, 0), color),
        VertexFormat(corner + glm::vec3(length, length, 0), color),
        VertexFormat(corner + glm::vec3(0, length, 0), color),
        VertexFormat(corner + glm::vec3(length, -length / 4, 0), color),
        VertexFormat(corner + glm::vec3(length,  5 *length / 4, 0), color),
        VertexFormat(corner + glm::vec3(length + length / 2,  length / 2, 0), color)
    };

    Mesh* square = new Mesh(name);
    std::vector<unsigned int> indices = { 0, 1, 2, 0, 2, 3, 4, 6, 5 };



    if (!fill) {
        square->SetDrawMode(GL_LINE_LOOP);
    }
    else {
        indices.push_back(0);
        indices.push_back(2);
    }

    square->InitFromData(vertices, indices);
    return square;
}

//functie ce creeaza o bila
Mesh* object2D::CreateBall(
    const std::string& name,
    glm::vec3 center,
    float radius,
    glm::vec3 color,
    bool fill)
{
    std::vector<VertexFormat> vertices;
    std::vector<unsigned int> indices;
    int numSegments = 100;

    vertices.push_back(VertexFormat(center, color));
    int centerIndex = 0;

    //adauga varfurile de pe circumferinta
    for (int i = 0; i <= numSegments; ++i) {
        float angle = (float)i * 2.0f * (float)M_PI / numSegments;
        glm::vec3 pos = center + glm::vec3(cos(angle) * radius, sin(angle) * radius, 0);
        vertices.push_back(VertexFormat(pos, color));
    }

    //adauga indecsii pentru triunghiurile cercului
    for (int i = 1; i <= numSegments; ++i) {
        indices.push_back(centerIndex);
        indices.push_back(i);
        indices.push_back(i + 1);
    }

    //creeaza mesh-ul
    Mesh* circle = new Mesh(name);
    if (!fill) {
        circle->SetDrawMode(GL_LINE_LOOP);
    }
    else {
        indices.push_back(0);
        indices.push_back(2);
    }
    circle->InitFromData(vertices, indices);
    return circle;
}