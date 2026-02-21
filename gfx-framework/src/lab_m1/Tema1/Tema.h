#pragma once

#include "components/simple_scene.h"
#include "components/text_renderer.h"
#include <string.h>
#include <string>
#include <iostream> 
#include <vector>
#include <queue> 
#include <set> 
#include <utility>
#include <cmath> 
#include <cstdlib>


namespace m1
{
    class Tema : public gfxc::SimpleScene
    {
    public:
        struct ViewportSpace
        {
            ViewportSpace() : x(0), y(0), width(1), height(1) {}
            ViewportSpace(int x, int y, int width, int height)
                : x(x), y(y), width(width), height(height) {
            }
            int x;
            int y;
            int width;
            int height;
        };

        struct LogicSpace
        {
            LogicSpace() : x(0), y(0), width(1), height(1) {}
            LogicSpace(float x, float y, float width, float height)
                : x(x), y(y), width(width), height(height) {
            }
            float x;
            float y;
            float width;
            float height;
        };

        struct Particle {
            glm::vec2 pos;        // Poziția
            glm::vec2 vel;        // Viteza
            float size;           // Dimensiunea
            std::string meshName; // Ce mesh să folosească (particle1, particle2, etc.)
        };

    public:
        Tema();
        ~Tema();

        void Init() override;

    private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void DrawStart(glm::mat3 visMatrix);
        void DrawGrid(glm::mat3 visMatrix);
		void DrawGame(glm::mat3 visMatrix, float deltaTimeSeconds);
        void Tema::DrawShip(glm::mat3 visMatrix, float deltaTimeSeconds);
        void Tema::DrawBall(glm::mat3 visMatrix, float deltaTimeSeconds);
        void Tema::HandleBallBlockCollision(float ballSize);
        void Tema::HandleBallUpperBlocksCollision(float ballSize);
        void Tema::DrawFinal (glm::mat3 visMatrix);
        void Tema::DrawPause(float deltaTimeSeconds);

        void OnKeyPress(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;

        // Sets the logic space and view space
        // logicSpace: { x, y, width, height }
        // viewSpace: { x, y, width, height }
        glm::mat3 VisualizationTransf2D(const LogicSpace& logicSpace, const ViewportSpace& viewSpace);
        glm::mat3 VisualizationTransf2DUnif(const LogicSpace& logicSpace, const ViewportSpace& viewSpace);

        void SetViewportArea(const ViewportSpace& viewSpace, glm::vec3 colorColor = glm::vec3(0), bool clear = true);

    protected:
        
        ViewportSpace viewSpace;
        LogicSpace logicSpace;
        glm::mat3 modelMatrix, visMatrix;

        glm::vec2 scale_engine;
        glm::vec2 scale_square;
        glm::vec2 scale_bumper;
        glm::vec2 scale_gun;

        glm::ivec2 resolution;
        glm::vec3 shipColour;
		glm::vec3 green = glm::vec3(0, 1, 0);
		glm::vec3 red = glm::vec3(1, 0, 0);
		glm::vec3 blue = glm::vec3(0, 0, 1);
		glm::vec3 yellow = glm::vec3(1, 1, 0);
		glm::vec3 orange = glm::vec3(1, 0.5f, 0);
		glm::vec3 purple = glm::vec3(0.5f, 0, 0.5f);
		glm::vec3 navyblue = glm::vec3(0, 0, 0.5f);
		glm::vec3 white = glm::vec3(1, 1, 1);
        glm::vec3 colourRemaining, colourButton;

        std::string todrag;
        std::pair<float, int> gridMatrix[12][8];
        std::pair<int, glm::vec2> blockMatrix[10][5];
        std::vector<Particle> particles;

        gfxc::TextRenderer* textRenderer;

        int status = 1;
        int movement = 1;
        int score = 0;
        int lives = 3;
        int numBlocks = 0;
        int blocksUsed = 0;
        int blockRemaining;
        int numParticles;

        float length;
        float aspectWindow;
        float arrowX, arrowY;
        float arrowW = 3.0f;
        float arrowH = 1.5f;
		float timePassed = 0.0f;
		float mouse_X, mouse_Y;
		float bumperX, bumperY;
		float engineX, engineY;
		float gunX, gunY;
		float squareX, squareY;
		float offsetX = 0;
		float offsetY = 0;
        float gridStartX = 6.5f;
		float gridStartY = 0.3f;
        float blockScale[10][5];
        float brickShrinkSpeed;
        float shipPosX;
        float shipPosY;
        float shipLength, shipHeight;
        float shipSpeed;
        float ballX, ballY;
        float minJ, minI;
        float ballDirY, ballDirX;
        float ballSpeed;
        float shakeTimer;
        float shakeMagnitude;
        float shakeTime;
        
        bool isDragging = false;
        bool validity = false;
		bool fillMode = true;
        bool isConnected;
        bool learning;
        bool started;
        bool isPaused = false;
    };

}   // namespace m1
