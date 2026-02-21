        #include "lab_m1/Tema1/Tema.h"

        #include <vector>
        #include <iostream>

        #include "lab_m1/Tema1/transform2D.h"
        #include "lab_m1/Tema1/object2D.h"

        using namespace std;
        using namespace m1;


        /*
         *  To find out more about `FrameStart`, `Update`, `FrameEnd`
         *  and the order in which they are called, see `world.cpp`.
         */


        Tema::Tema()
        {
        }


        Tema::~Tema()
        {
        }


        void Tema::Init()
        {
            glm::ivec2 resolution = window->GetResolution();
            auto camera = GetSceneCamera();
            camera->SetOrthographic(0, (float)resolution.x, 0, (float)resolution.y, 0.01f, 400);
            camera->SetPosition(glm::vec3(0, 0, 50));
            camera->SetRotation(glm::vec3(0, 0, 0));
            camera->Update();
            GetCameraInput()->SetActive(false);

            std::string fontPath = PATH_JOIN(window->props.selfDir, RESOURCE_PATH::FONTS, "Hack-Bold.ttf");
            textRenderer = new gfxc::TextRenderer(window->props.selfDir, resolution.x, resolution.y);
            textRenderer->Load(fontPath, 50);

            resolution = window->GetResolution(false);
	        aspectWindow = resolution.x / (float)resolution.y;
            glm::vec3 corner = glm::vec3(0, 0, 0);

            shipColour = orange;
            colourButton = red;
            colourRemaining = green;

            brickShrinkSpeed = 2.0f;
            shakeTime = 0.1f;
            shakeMagnitude = 0.1f;
            shakeTimer = 0.0f;
            arrowW = 3.0f;
            arrowH = 1.5f;
            logicSpace.x = 0.0f;
            logicSpace.y = 0.0f;
            logicSpace.width = 20.0f;
            logicSpace.height = aspectWindow * 20.0f;
            arrowX = logicSpace.width / 2 - length * 2;
            arrowY = logicSpace.height * 2 / 4 - length;
            shipPosX = logicSpace.width / 2.0f;
            shipSpeed = 12.0f;

            status = 1;
            length = 1;

            learning = true;
            started = false;
			//initializare meshes
            Mesh* square = object2D::CreateBlock("square", corner, length, shipColour, true);
            AddMeshToList(square);

	        Mesh* gun = object2D::CreateGun("gun", glm::vec3(0, 0, 0), length, white, white, glm::vec3(0.2f, 0.2f, 0.2f), true);
            AddMeshToList(gun);

            Mesh* bumper = object2D::CreateBumper("bumper", glm::vec3(0, 0, 0), length, shipColour, glm::vec3(0.96f, 0.96f, 0.86f), true);
            AddMeshToList(bumper);

	        Mesh* engine = object2D::CreateEngine("engine", corner, length, shipColour, true);
	        AddMeshToList(engine);

	        Mesh* emptySquare = object2D::CreateBlock("emptySquare", corner, length, glm::vec3(1, 0, 0), false);
	        AddMeshToList(emptySquare);

	        Mesh* gridSquare = object2D::CreateBlock("gridSquare", corner,length, glm::vec3(0, 0, 1), true);
	        AddMeshToList(gridSquare);

	        Mesh* remainingGridSquare = object2D::CreateBlock("remainingGridSquare", corner, length, colourRemaining, true);
	        AddMeshToList(remainingGridSquare);

	        Mesh* button = object2D::CreateButton("button", corner, length, colourButton, true);
            AddMeshToList(button);

	        Mesh* gridMargin = object2D::CreateBlock("gridMargin", corner, length, colourButton, false);
	        AddMeshToList(gridMargin);

            Mesh* arrow = object2D::CreateArrow("arrow", corner, length, glm::vec3(1, 0.5f, 0), true);
            AddMeshToList(arrow);

		    Mesh* redSquare = object2D::CreateBlock("redSquare", corner, length, red, true);
		    AddMeshToList(redSquare);
		    
            Mesh* greenSquare = object2D::CreateBlock("greenSquare", corner, length, green, true);
		    AddMeshToList(greenSquare);
		    
            Mesh* blueSquare = object2D::CreateBlock("blueSquare", corner, length, blue, true);
		    AddMeshToList(blueSquare);
		    
            Mesh* yellowSquare = object2D::CreateBlock("yellowSquare", corner, length, yellow, true);
		    AddMeshToList(yellowSquare);
		   
            Mesh* orangeSquare = object2D::CreateBlock("orangeSquare", corner, length, orange, true);
		    AddMeshToList(orangeSquare);
		    
            Mesh* purpleSquare = object2D::CreateBlock("purpleSquare", corner, length, purple, true);
		    AddMeshToList(purpleSquare);
			
            Mesh* whiteSquare = object2D::CreateBlock("whiteSquare", corner, length, white, true);
			AddMeshToList(whiteSquare);

			Mesh* ball = object2D::CreateBall("ball", corner, length, white, true);
			AddMeshToList(ball);

            Mesh* p1 = object2D::CreateBall("particle1", corner, length, glm::vec3(0.8f, 0.8f, 0.8f), true);
            AddMeshToList(p1);

            Mesh* p2 = object2D::CreateBall("particle2", corner, length, glm::vec3(0.5f, 0.5f, 0.5f), true);
            AddMeshToList(p2);

            Mesh* p3 = object2D::CreateBall("particle3", corner, length, glm::vec3(0.3f, 0.3f, 0.3f), true);
            AddMeshToList(p3);

            std::vector<std::string> particleMeshes = { "particle1", "particle2", "particle3" };

            //initializam particulele
            numParticles = 200;
            for (int i = 0; i < numParticles; i++) {
                Particle p;

                p.pos.x = (rand() / (float)RAND_MAX) * logicSpace.width;
                p.pos.y = (rand() / (float)RAND_MAX) * logicSpace.height;

                p.vel.x = 0;
                p.vel.y = -((rand() / (float)RAND_MAX) * 2.0f + 0.5f);

                p.size = (rand() / (float)RAND_MAX) * 0.02f + 0.01f;
                p.meshName = particleMeshes[rand() % particleMeshes.size()];

                particles.push_back(p);
            }
        }

        // 2D visualization matrix
        glm::mat3 Tema::VisualizationTransf2D(const LogicSpace& logicSpace, const ViewportSpace& viewSpace)
        {
            float sx, sy, tx, ty;
            sx = viewSpace.width / logicSpace.width;
            sy = viewSpace.height / logicSpace.height;
            tx = viewSpace.x - sx * logicSpace.x;
            ty = viewSpace.y - sy * logicSpace.y;

            return glm::transpose(glm::mat3(
                sx, 0.0f, tx,
                0.0f, sy, ty,
                0.0f, 0.0f, 1.0f));
        }


        // Uniform 2D visualization matrix (same scale factor on x and y axes)
        glm::mat3 Tema::VisualizationTransf2DUnif(const LogicSpace& logicSpace, const ViewportSpace& viewSpace)
        {
            float sx, sy, tx, ty, smin;
            sx = viewSpace.width / logicSpace.width;
            sy = viewSpace.height / logicSpace.height;
            if (sx < sy)
                smin = sx;
            else
                smin = sy;
            tx = viewSpace.x - smin * logicSpace.x + (viewSpace.width - smin * logicSpace.width) / 2;
            ty = viewSpace.y - smin * logicSpace.y + (viewSpace.height - smin * logicSpace.height) / 2;

            return glm::transpose(glm::mat3(
                smin, 0.0f, tx,
                0.0f, smin, ty,
                0.0f, 0.0f, 1.0f));
        }


        void Tema::SetViewportArea(const ViewportSpace& viewSpace, glm::vec3 colorColor, bool clear)
        {
            glViewport(viewSpace.x, viewSpace.y, viewSpace.width, viewSpace.height);

            glEnable(GL_SCISSOR_TEST);
            glScissor(viewSpace.x, viewSpace.y, viewSpace.width, viewSpace.height);

            // Clears the color buffer (using the previously set color) and depth buffer
            glClearColor(colorColor.r, colorColor.g, colorColor.b, 1);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glDisable(GL_SCISSOR_TEST);

            GetSceneCamera()->SetOrthographic((float)viewSpace.x, (float)(viewSpace.x + viewSpace.width), (float)viewSpace.y, (float)(viewSpace.y + viewSpace.height), 0.1f, 400);
            GetSceneCamera()->Update();
        }


        void Tema::FrameStart()
        {
            // Clears the color buffer (using the previously set color) and depth buffer
            glClearColor(0, 0, 0, 1);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }

        void Tema::DrawPause(float deltaTimeSeconds)
        {
            SetViewportArea(viewSpace, glm::vec3(0, 0, 0.1f), true);
            // Text mare alb
            glDisable(GL_DEPTH_TEST);
            //randam particulele pe fundal
            for (auto& p : particles) {
                //actualizeaza pozitia particulei
                p.pos.y += p.vel.y * deltaTimeSeconds;
                //verifica daca particula a iesit din ecran
                //daca iese o randam iar sus
                if (p.pos.y < -p.size) {
                    p.pos.y = logicSpace.height;
                    p.pos.x = (rand() / (float)RAND_MAX) * logicSpace.width;
                }
                //desenam particula
                modelMatrix = visMatrix
                    * transform2D::Translate(p.pos.x, p.pos.y)
                    * transform2D::Scale(p.size, p.size);
                RenderMesh2D(meshes[p.meshName], shaders["VertexColor"], modelMatrix);
            }
            glEnable(GL_DEPTH_TEST);
            textRenderer->RenderText("PAUSED", 350, 300, 3.0f, glm::vec3(1, 1, 1));
        }


        void Tema::Update(float deltaTimeSeconds)
        {
			
            if (isPaused)
                DrawPause(deltaTimeSeconds);
            else {          //preluam rezolutia actuala a ferestrei
                resolution = window->GetResolution();

                //ajustam logic-space pentru a curata offset-ul pe X
                aspectWindow = resolution.x / (float)resolution.y;
                logicSpace.height = logicSpace.width / aspectWindow;

                viewSpace = ViewportSpace(0, 0, resolution.x, resolution.y);

                visMatrix = glm::mat3(1);
                visMatrix *= VisualizationTransf2D(logicSpace, viewSpace);
                //efect de shake la distrugerea unui bloc
                if (shakeTimer > 0)
                {
                    shakeTimer -= deltaTimeSeconds;
                    if (shakeTimer < 0)
                        shakeTimer = 0;

                    float shakeX = ((rand() / (float)RAND_MAX) * 2.0f - 1.0f) * shakeMagnitude;
                    float shakeY = ((rand() / (float)RAND_MAX) * 2.0f - 1.0f) * shakeMagnitude;

                    visMatrix *= transform2D::Translate(shakeX, shakeY);
                }
                //verificam starea jocului si desenam scena corespunzatoare
                if (status == 1)
                    DrawStart(visMatrix);
                else if (status == 2)
                    DrawGrid(visMatrix);
                else if (status == 3)
                    DrawGame(visMatrix, deltaTimeSeconds);
                else if (status == 4)
                    DrawFinal(visMatrix);
                //actualizam timpul scurs
                timePassed += deltaTimeSeconds;
            }

        }

        void Tema::FrameEnd()
        {
        }

		//functie ce se ocupa de coliziunea dintre bila si blocurile din partea superioara
        void Tema::HandleBallUpperBlocksCollision(float ballSize)
        {
            int blockDimension = (int) logicSpace.width / 10;
            float bw = 1.9f;
            float bh = 0.8f;
			//parcurgem matricea de blocuri
            for (int i = 0; i < 10; i++) {
                for (int j = 0; j < 5; j++) {
					//preluam valoarea blocului (numarul de vieti ramase)
                    int& value = blockMatrix[i][j].first;

                    // ignora blocurile deja sparte sau care se sparg
                    if (value <= 0) continue;

                    // pozitia si dimensiunea blocului
                    float bx = i * blockDimension + 0.05f;
                    float by = logicSpace.height - (j * 0.85f + 2);
                    float left = bx;
                    float right = bx + bw;
                    float bottom = by;
                    float top = by + bh;

                    // verificare coliziune
                    if (ballX + ballSize < left) 
                        continue;
                    if (ballX - ballSize > right) 
                        continue;
                    if (ballY + ballSize < bottom) 
                        continue;
                    if (ballY - ballSize > top) 
                        continue;

                    value--;
                    score++;

					// daca blocul s-a spart, pornim efectul de shake
                    if (value == 0) {
                        shakeTimer = shakeTime;
                    }

                    //calculam centrul bloccului
                    float blockCenterX = left + bw / 2.0f;
                    float blockCenterY = bottom + bh / 2.0f;

					//calculam distanta dintre centrul bilei si centrul blocului
                    float dx = ballX - blockCenterX;
                    float dy = ballY - blockCenterY;

					//calculam dimensiunile combinate
                    float combinedHalfWidth = bw / 2.0f + ballSize;
                    float combinedHalfHeight = bh / 2.0f + ballSize;

					//calculam cat se suprapun pe fiecare axa
                    float overlapX = combinedHalfWidth - abs(dx);
                    float overlapY = combinedHalfHeight - abs(dy);

                    // decidem pe ce axa a fost coliziunea
                    // axa cu suprapunerea MAI MICa este axa coliziunii
                    if (overlapX < overlapY) {
                        // coliziune orizontala
                        ballDirX *= -1;
                        //corectare pozitie (previne "lipirea" bilei de bloc)
                        if (dx > 0) {
                            ballX = right + ballSize;
                        }
                        else {
                            ballX = left - ballSize;
                        }
                    }
                    else {
                        //coliziune verticala
                        ballDirY *= -1;
                        if (dy > 0) {
                            ballY = top + ballSize;
                        }
                        else {
                            ballY = bottom - ballSize;
                        }
                    }
                    return;
                }
            }
        }

		//functie de desenare a bilei
        void Tema::DrawBall(glm::mat3 visMatrix, float deltaTimeSeconds)
        {
            float ballSize = 0.15f;

            // mingea sta pe nava pana incepe jocul
            if (!started) {
                ballX = shipPosX;
                ballY = shipPosY + shipHeight + ballSize;
            }
            else {
				// daca am inceput, actualizam pozitia bilei
                ballX += ballDirX * ballSpeed * deltaTimeSeconds;
                ballY += ballDirY * ballSpeed * deltaTimeSeconds;

				// coliziuni cu peretii laterali, corectam pozitia si inversam directia
                if (ballX - ballSize < 0) {
                    ballX = 0 + ballSize;
                    ballDirX *= -1;
                }
                else if (ballX + ballSize > logicSpace.width) {
                    ballX = logicSpace.width - ballSize;
                    ballDirX *= -1;
                }
				// coliziune cu tavanul spatilui de joc, nu al ferestrei
                if (ballY + ballSize > logicSpace.height - 1) {
                    ballY = logicSpace.height - ballSize;
                    ballDirY *= -1;
                }

				// daca am trecut sub nava, pierdem o viata si resetam bila
                if (ballY < shipPosY + shipHeight + ballSize / 2 - 0.25) {
                    lives--;
                    started = false;
                    shipPosX = logicSpace.width / 2;
                    return;
                }
				//coliziunea cu nava
                float paddleTop = shipPosY + shipHeight + ballSize / 2;
                float paddleLeft = shipPosX - shipLength / 2;
                float paddleRight = shipPosX + shipLength / 2;

                if (ballY - ballSize <= paddleTop &&
                    ballX >= paddleLeft &&
                    ballX <= paddleRight &&
                    ballDirY < 0)
                {
					//pozitia relativa a bilei
                    float relativeX = (ballX - shipPosX) / (shipLength / 2);

                    //unghi maxim (60°)
                    float maxAngle = M_PI / 3.0f;
                    float angle = relativeX * maxAngle;

                    //directia bilei
                    ballDirX = sin(angle);
                    ballDirY = cos(angle);

                    // ne asiguram ca sarim in sus
                    ballDirY = fabs(ballDirY);
                }

                // coliziuni cu blocurile
                HandleBallUpperBlocksCollision(ballSize);
            }

            modelMatrix = visMatrix
                * transform2D::Translate(ballX, ballY)
                * transform2D::Scale(ballSize, ballSize);

            RenderMesh2D(meshes["ball"], shaders["VertexColor"], modelMatrix);
        }

		//functie de desenare a navei
        void Tema::DrawShip(glm::mat3 visMatrix, float deltaTimeSeconds) {
            float blockSize = 0.6f;

            //mutare pe axa X cu tasta stanga/dreapta
            if (window->KeyHold(GLFW_KEY_LEFT) && started)
                shipPosX -= shipSpeed * deltaTimeSeconds;
            if (window->KeyHold(GLFW_KEY_RIGHT) && started)
                shipPosX += shipSpeed * deltaTimeSeconds;

            //determinam dimensiunea efectiva a navei
            int minI = 12, maxI = -1;
            int minJ = 8, maxJ = -1;
            for (int i = 0; i < 12; i++) {
                for (int j = 0; j < 8; j++) {
                    if (gridMatrix[i][j].second != 0) {
                        if (i < minI) minI = i;
                        if (i > maxI) maxI = i;
                        if (j < minJ) minJ = j;
                        if (j > maxJ) maxJ = j;
                    }
                }
            }

            shipLength = (maxI - minI + 1) * blockSize;
			shipHeight = (maxJ - minJ + 1) * blockSize;

            //limitam nava sa nu iasa din ecran
            if (shipPosX - shipLength / 2 < 0) shipPosX = shipLength / 2;
            if (shipPosX + shipLength / 2 > logicSpace.width) shipPosX = logicSpace.width - shipLength / 2;

            //pozitia coltului stanga-jos al navei
            float startX = shipPosX - shipLength / 2.0f;
            float startY = logicSpace.height / 10.0f - ((maxJ - minJ + 1) * blockSize) / 2.0f;
            shipPosY = startY;

            //desenam fiecare bloc al navei
			//parcurgem matricea de blocuri din grid
            for (int i = 0; i < 12; i++) {
                for (int j = 0; j < 8; j++) {
					//preluam tipul blocului
                    int type = gridMatrix[i][j].second;
                    if (type == 0) continue;
					//calculam pozitia blocului
                    float shipX = startX + (i - minI) * blockSize;
                    float shipY = startY + (j - minJ) * blockSize;
					//randam blocul corespunzator tipului   
                    modelMatrix = visMatrix
                        * transform2D::Translate(shipX, shipY)
                        * transform2D::Scale(blockSize, blockSize);
                    switch (type) {
                    case 1: RenderMesh2D(meshes["engine"], shaders["VertexColor"], modelMatrix); break;
                    case 2: RenderMesh2D(meshes["bumper"], shaders["VertexColor"], modelMatrix); break;
                    case 3: RenderMesh2D(meshes["square"], shaders["VertexColor"], modelMatrix); break;
                    case 4: RenderMesh2D(meshes["gun"], shaders["VertexColor"], modelMatrix); break;
                    }
                }
            }
        }

		//deseneaza ecranul de start
        void Tema::DrawStart(glm::mat3 visMatrix)
        {
            SetViewportArea(viewSpace, glm::vec3(0.9f), true);

            // mica variatie sinusoidala
            float offsetY = sin(timePassed * 2.0f) * 0.15f;
            float offsetX = cos(timePassed * 1.7f) * 0.1f;
			//randam partile navei ce oscileaza usor din ecranul de start
			//motorul
            modelMatrix = visMatrix * transform2D::Translate(4.5f + offsetX, 3 + offsetY) * transform2D::Rotate(glm::radians(45.0f)) * transform2D::Scale(2, 2);
            RenderMesh2D(meshes["engine"], shaders["VertexColor"], modelMatrix);
            //tunul
            modelMatrix = visMatrix * transform2D::Translate(3.5f + sin(timePassed * 2.2f) * 0.1f, 11 + cos(timePassed * 2.2f) * 0.15f) * transform2D::Rotate(glm::radians(135.0f)) * transform2D::Scale(1.75f, 1.75f);
            RenderMesh2D(meshes["gun"], shaders["VertexColor"], modelMatrix);
			//bumper-ul
            modelMatrix = visMatrix * transform2D::Translate(14 + sin(timePassed * 1.8f) * 0.2f, 1.5f + offsetY) * transform2D::Rotate(glm::radians(-45.0f)) * transform2D::Scale(2, 2);
            RenderMesh2D(meshes["square"], shaders["VertexColor"], modelMatrix);
            //patratul
            modelMatrix = visMatrix * transform2D::Translate(16, 8.5f + sin(timePassed * 2.5f) * 0.2f) * transform2D::Rotate(glm::radians(-135.0f)) * transform2D::Scale(2, 2);
            RenderMesh2D(meshes["bumper"], shaders["VertexColor"], modelMatrix);
			//sageata de start
            arrowX = logicSpace.width / 2 - length * 2;
            arrowY = logicSpace.height * 2 / 4 - length;
			//daca mouse-ul este deasupra sagetii, oprim miscarea acesteia
		    if (movement == 1)
                modelMatrix = visMatrix * transform2D::Translate(arrowX, arrowY + sin(timePassed * 3.0f) * 0.1f) * transform2D::Scale(arrowW, arrowH);
            else 
			    modelMatrix = visMatrix * transform2D::Translate(arrowX, arrowY) * transform2D::Scale(arrowW, arrowH);
            RenderMesh2D(meshes["arrow"], shaders["VertexColor"], modelMatrix);
			// desenam textul de START deasupra sagetii
            glm::vec3 arrowScreen = visMatrix * glm::vec3(arrowX, arrowY, 1);
            glDisable(GL_DEPTH_TEST);
            textRenderer->RenderText("START", 530, 350, 1.33f, white);
            glEnable(GL_DEPTH_TEST);
        }

		//functie de desenare a ecranului cu grid-ul de constructie
        void Tema::DrawGrid(glm::mat3 visMatrix) {
            SetViewportArea(viewSpace, navyblue, true);
			//latimea si inaltimea unui patrat gol din grid
            float cellW = 0.25f * logicSpace.width;
            float cellH = 0.25f * logicSpace.height;
			//randam cele 4 patrate goale din stanga
            for (int i = 0; i < 4; i++) {
                float y = i * cellH;

                modelMatrix = visMatrix * transform2D::Translate(0, y) * transform2D::Scale(cellW, cellH);
                RenderMesh2D(meshes["emptySquare"], shaders["VertexColor"], modelMatrix);
				//in ele randam obiectele ce pot fi alese
                switch (i) {
                case 0: // motorul
                    modelMatrix = visMatrix * transform2D::Translate(0, y) * transform2D::Scale(0.33f * cellH, 0.33f * cellH) * transform2D::Translate(0.45f * cellW, 0.4f * cellH);
                    scale_engine = glm::vec2(0.33f * cellH, 0.33f * cellH);
                    RenderMesh2D(meshes["engine"], shaders["VertexColor"], modelMatrix);
                    break;

				case 1: // bumper-ul
                    modelMatrix = visMatrix * transform2D::Translate(0, y) * transform2D::Scale(0.25f * cellH, 0.25f * cellH)  * transform2D::Translate(0.6f * cellW, 0.33f * cellH);
                    scale_bumper = glm::vec2(0.25f * cellH, 0.25f * cellH);
                    RenderMesh2D(meshes["bumper"], shaders["VertexColor"], modelMatrix);
                    break;

                case 2: // patrat
                    modelMatrix = visMatrix * transform2D::Translate(0, y) * transform2D::Scale(0.33f * cellW, 0.33f * cellW) * transform2D::Translate(0.2f * cellW, 0.15f * cellH);
                    scale_square = glm::vec2(0.33f * cellW, 0.33f * cellW);
                    RenderMesh2D(meshes["square"], shaders["VertexColor"], modelMatrix);
                    break;

                case 3: //tunul
                    modelMatrix = visMatrix * transform2D::Translate(0, y) * transform2D::Scale(0.25f * cellH, 0.25f * cellH) * transform2D::Translate(0.59f * cellW, 0.2f * cellH);
                    scale_gun = glm::vec2(0.25f * cellH, 0.25f * cellH);
                    RenderMesh2D(meshes["gun"], shaders["VertexColor"], modelMatrix);
                    break;
                }

                //gridul de 8*10 patrate albastastre de dimensiune 1 x 1
				//daca suntem in modul de umplere, salvam pozitia x a patratului
                for (int i = 0; i < 12; i++) {
                    for (int j = 0; j < 8; j++) {
                        modelMatrix = visMatrix * transform2D::Translate(i * 1.05f + gridStartX, j * 1.05f + gridStartY);
                        RenderMesh2D(meshes["gridSquare"], shaders["VertexColor"], modelMatrix);
                        if (fillMode) {
                            gridMatrix[i][j].first = i * 1.05f + gridStartX;
                        }
                    }
                }
				//pentru a umple doar odata un patrat, dezactivam fillMode
                fillMode = false;
				//randam patratele ce ajuta la selectia culorilor
                //rosu
                modelMatrix = visMatrix * transform2D::Translate(5.2f, logicSpace.height - 4) * transform2D::Scale(1, 1);
                RenderMesh2D(meshes["redSquare"], shaders["VertexColor"], modelMatrix);
				//albastru
                modelMatrix = visMatrix * transform2D::Translate(5.2f, logicSpace.height - 5.1f) * transform2D::Scale(1, 1);
                RenderMesh2D(meshes["blueSquare"], shaders["VertexColor"], modelMatrix);
				//verde
                modelMatrix = visMatrix * transform2D::Translate(5.2f, logicSpace.height - 6.2f) * transform2D::Scale(1, 1);
                RenderMesh2D(meshes["greenSquare"], shaders["VertexColor"], modelMatrix);
				//galben
                modelMatrix = visMatrix * transform2D::Translate(5.2f, logicSpace.height - 7.3f) * transform2D::Scale(1, 1);
                RenderMesh2D(meshes["yellowSquare"], shaders["VertexColor"], modelMatrix);
				//orange
                modelMatrix = visMatrix * transform2D::Translate(5.2f, logicSpace.height - 8.4f) * transform2D::Scale(1, 1);
                RenderMesh2D(meshes["orangeSquare"], shaders["VertexColor"], modelMatrix);
				//mov
                modelMatrix = visMatrix * transform2D::Translate(5.2f, logicSpace.height - 9.5f) * transform2D::Scale(1, 1);
                RenderMesh2D(meshes["purpleSquare"], shaders["VertexColor"], modelMatrix);

                //patratele ce inidca cate blocuri au mai ramas disponibile
                for (int i = 0; i < 10 - blocksUsed; i++) {
                    modelMatrix = visMatrix * transform2D::Translate(i * 1.3f + 5.3f, logicSpace.height - 1.7f)  * transform2D::Scale(1.15f, 1.15f);
                    RenderMesh2D(meshes["remainingGridSquare"], shaders["VertexColor"], modelMatrix);
                }
                // calculam cate blocuri au fost folosite
                blocksUsed = 0;
                for (int i = 0; i < 12; i++) {
                    for (int j = 0; j < 8; j++) {
                        if (gridMatrix[i][j].second != 0 && gridMatrix[i][j].second != 5)
                            blocksUsed++;
                    }
                }
				//daca mai avem numar de blocuri disponibile, verificam daca sunt conectate
                if (blocksUsed == 0 || blocksUsed > 10) {
                    colourButton = red;
                    isConnected = false;
                }
                else {
					//folosind BFS verificam daca toate blocurile sunt conectate
                    vector<pair<int, int>> blocks;
                    for (int i = 0; i < 12; i++)
                        for (int j = 0; j < 8; j++)
                            if (gridMatrix[i][j].second != 0 && gridMatrix[i][j].second != 5)
                                blocks.push_back({ i,j });
					//initializam coada si setul de vizitate
                    queue<pair<int, int>> q;
                    set<pair<int, int>> vis;
                    q.push(blocks[0]);
                    vis.insert(blocks[0]);
					//incepem parcurgerea BFS
                    while (!q.empty()) {
                        auto cur = q.front(); q.pop();
                        int cx = cur.first, cy = cur.second;
                        int dirs[4][2] = { {1,0},{-1,0},{0,1},{0,-1} };
                        for (auto& d : dirs) {
                            int nx = cx + d[0], ny = cy + d[1];
                            if (nx >= 0 && nx < 12 && ny >= 0 && ny < 8 && gridMatrix[nx][ny].second != 0 && gridMatrix[nx][ny].second != 5) {
                                if (!vis.count({ nx,ny })) {
                                    vis.insert({ nx,ny });
                                    q.push({ nx,ny });
                                }
                            }
                        }
                    }
					// daca numarul de blocuri vizitate este egal cu numarul total de blocuri, atunci sunt conectate
                    isConnected = (vis.size() == blocks.size());
					// actualizam culoarea butonului in functie de starea de conectare
                    colourButton = isConnected ? green : red;
                }
                // desenam butonul cu culoarea actualizata
                float posX = logicSpace.width - 1.7f;
                float posY = logicSpace.height - 1.7f;
                modelMatrix = visMatrix * transform2D::Translate(posX, posY) * transform2D::Scale(1.15f, 1.15f);
                meshes["button"] = object2D::CreateButton("button", glm::vec3(0, 0, 0), length, colourButton, true);
                RenderMesh2D(meshes["button"], shaders["VertexColor"], modelMatrix);

				//contur pentru gridul de patrate de aceeasi culoare cu butonul
                 modelMatrix = visMatrix * transform2D::Translate(6.4f, 0.2f) * transform2D::Scale(13.3f, 8.9f);
				 meshes["gridMargin"] = object2D::CreateBlock("gridMargin", glm::vec3(0, 0, 0), length, colourButton, false);
                 RenderMesh2D(meshes["gridMargin"], shaders["VertexColor"], modelMatrix);
				 //daca un obiect este tras de mouse, il randam sub mouse
                 if (isDragging) {
					 // calculam pozitia de drag
                     float dragX = mouse_X - offsetX;
                     float dragY = mouse_Y - offsetY;

                     glm::mat3 dragMatrix = visMatrix * transform2D::Translate(dragX, dragY);
                     glDisable(GL_DEPTH_TEST);
					 //in functie de obiectul tras, il randam corespunzator
                     if (todrag == "engine") {
                         dragMatrix *= transform2D::Scale(scale_engine.x, scale_engine.y);
                         RenderMesh2D(meshes["engine"], shaders["VertexColor"], dragMatrix);
                     }
                     else if (todrag == "bumper") {
                         dragMatrix *= transform2D::Scale(scale_bumper.x, scale_bumper.y);
                         RenderMesh2D(meshes["bumper"], shaders["VertexColor"], dragMatrix);
                     }
                     else if (todrag == "square") {
                         dragMatrix *= transform2D::Scale(scale_square.x, scale_square.y);
                         RenderMesh2D(meshes["square"], shaders["VertexColor"], dragMatrix);
                     }
                     else if (todrag == "gun") {
                         dragMatrix *= transform2D::Scale(scale_gun.x, scale_gun.y);
                         RenderMesh2D(meshes["gun"], shaders["VertexColor"], dragMatrix);
                     }
					 glEnable(GL_DEPTH_TEST);
                 }
				 blocksUsed = 0;
				 glDisable(GL_DEPTH_TEST);
				 //randam blocurile plasate in grid
                 for (int i = 0; i < 12; i++) {
                     for (int j = 0; j < 8; j++) {
						 //ignoram blocurile goale sau cele auxiliare in care valoarea este 0, respectiv 5
                         if(gridMatrix[i][j].second == 0 || gridMatrix[i][j].second == 5)
							 continue;
                         else {
							 //daca avem un tip valid de bloc, il randam
                             blocksUsed++;
                             float squareX = gridMatrix[i][j].first;
                             float squareY = j * 1.05f + gridStartY;
                             modelMatrix = visMatrix * transform2D::Translate(squareX, squareY) * transform2D::Scale(1, 1);
                             switch ((int)gridMatrix[i][j].second) {
                             case 1: RenderMesh2D(meshes["engine"], shaders["VertexColor"], modelMatrix); break;
                             case 2: RenderMesh2D(meshes["bumper"], shaders["VertexColor"], modelMatrix); break;
                             case 3: RenderMesh2D(meshes["square"], shaders["VertexColor"], modelMatrix); break;
                             case 4: RenderMesh2D(meshes["gun"], shaders["VertexColor"], modelMatrix); break;
                             }
                         }

                     }
                 }
				 glEnable(GL_DEPTH_TEST);
            }
        }
		//functie de desenare a ecranului final (castig sau pierdere)
        void Tema::DrawFinal(glm::mat3 visMatrix) {
			//in functie de rezultat, schimbam textul si culoarea acestuia
            std::string message = (blockRemaining == 0) ? "CONGRATULATION! YOU WON" : "GAME OVER";
			glm::vec3 colour = (blockRemaining == 0) ? glm::vec3(0.2f, 1.0f, 0.2f) : glm::vec3(1.0f, 0.2f, 0.2f);
            //setam variabilele necesare afisarii
            float scaleTitle = 1.25f;
            float titleWidth = message.length() * 0.5f * scaleTitle; // latime aproximativa per caracter
            float posX = (blockRemaining == 0) ? (logicSpace.width / 2.0f - titleWidth / 2.0f + 250) : (logicSpace.width / 2.0f - titleWidth / 2.0f + 450);
			//afisam textul principal
            textRenderer->RenderText(message, posX, logicSpace.height / 2.0f + 1.5f + 150, scaleTitle, colour);
			//afisam scorul final
            std::string scoreText = "Score: " + std::to_string(score);
            float scoreScale = 0.6f;
            float scoreWidth = scoreText.length() * 0.5f * scoreScale;
            textRenderer->RenderText(scoreText, logicSpace.width / 2.0f - scoreWidth / 2.0f + 550, logicSpace.height / 2.0f + 300, scoreScale, glm::vec3(1.0f, 1.0f, 1.0f));
			//afisam instructiunile pentru restart sau iesire
            std::string infoText = "Press SPACE to rebuild ship";
            float infoScale = 0.45f;
            float infoWidth = infoText.length() * 0.5f * infoScale;
            textRenderer->RenderText(infoText, logicSpace.width / 2.0f - infoWidth / 2.0f + 450, logicSpace.height / 2.0f - 2.0f + 450, infoScale, glm::vec3(0.8f, 0.8f, 0.8f));
            infoText = "Press ENTER to EXIT";
            textRenderer->RenderText(infoText, logicSpace.width / 2.0f - infoWidth / 2.0f + 500, logicSpace.height / 2.0f - 2.0f + 550, infoScale, glm::vec3(0.8f, 0.8f, 0.8f)
            );
			//logica de restart sau iesire
            if (window->KeyHold(GLFW_KEY_SPACE)) {
				//rsetam variabilele pentru un nou joc
                score = 0;
                lives = 3;
                started = false;

                for (int i = 0; i < 12; i++)
                    for (int j = 0; j < 8; j++)
                        gridMatrix[i][j].second = 0;
                ballX = shipPosX;
                ballY = shipPosY + shipHeight;
                status = 2;
            }
            if (window->KeyHold(GLFW_KEY_ENTER)) {
                exit(0);
            }
        }

		//functie de desenare a ecranului de joc
        void Tema::DrawGame(glm::mat3 visMatrix, float deltaTimeSeconds)  {
            SetViewportArea(viewSpace, glm::vec3(0, 0, 0.1f), true);
			//desenam particulele de fundal
            glDisable(GL_DEPTH_TEST);
            for (auto& p : particles) {
				//actualizeaza pozitia particulei
                p.pos.y += p.vel.y * deltaTimeSeconds;
				//verifica daca particula a iesit din ecran
                //daca iese o randam iar sus
                if (p.pos.y < -p.size) {
                    p.pos.y = logicSpace.height;
                    p.pos.x = (rand() / (float)RAND_MAX) * logicSpace.width;
                }
				//desenam particula
                modelMatrix = visMatrix
                    * transform2D::Translate(p.pos.x, p.pos.y)
                    * transform2D::Scale(p.size, p.size);
                RenderMesh2D(meshes[p.meshName], shaders["VertexColor"], modelMatrix);
            }
            glEnable(GL_DEPTH_TEST);
			//daca nu mai avem blocuri ramase sau vieti, trecem la ecranul final
            if (blockRemaining == 0 && lives > 0 || lives <= 0) {
                status = 4;
                return;
            } 
			//cu space incepem jocul
            if (window->KeyHold(GLFW_KEY_SPACE) && !started) {
                started = true;
                ballDirX = 0.707f;
                ballDirY = 0.707f;
                ballSpeed = 6.0f;
            }
            //afiseaza scorul si vietile
            string scoreText = "Score: " + to_string(score);
            string livesText = "Lives: " + to_string(lives);
            textRenderer->RenderText(scoreText, 25.0f, 15.0f, 0.35f, white);
            textRenderer->RenderText(livesText, 1170, 15.0f, 0.35f, white);
            int blockDimension = (int)logicSpace.width / 10;

            //randam blocurile de sus ce urmeaza a fi sparte 
            //le salvam int-o matrice ce le retine numarul de coliziuni pana la spargere
            // si pozitia
			float blockSizeX = 1.9f, blockSizeY = 0.8f;
			//populam matricea o singura data
            if (learning) {
                blockRemaining = 0;
                for (int i = 0; i < 10; i++) {
					//randam blocurile pe coloane si randuri de diverse culori si rezistente
                    for (int j = 0; j < 5; j++) {
                        modelMatrix = visMatrix * transform2D::Translate(i * blockDimension - 0.05f + 0.1f, logicSpace.height - (j * 0.85f + 2)) * transform2D::Scale(1.9f, 0.8f);
                        if (j == 0 || (i % 3 == 0 && j == 1))
                            blockMatrix[i][j] = { 3,  glm::vec2(i * blockDimension - 0.95f, logicSpace.height - (j * 0.85f + 2)) };
                        else if ((j == 1 && i % 3 != 0) || j == 2 && (i % 2 == 0 || i % 3 == 0) || (j == 3 && i % 3 == 0))
                            blockMatrix[i][j] = { 2,  glm::vec2(i * blockDimension - 0.95f, logicSpace.height - (j * 0.85f + 2)) };
                        else
                            blockMatrix[i][j] = { 1,  glm::vec2(i * blockDimension - 0.95f, logicSpace.height - (j * 0.85f + 2)) };
                        blockScale[i][j] = 1.0f;
						//contorizam cate blocuri avem in total
                        if (blockMatrix[i][j].first > 0) {
                            blockRemaining++;
                        }
                    }
                }
                learning = false;
            }
			//daca deja am populat matricea, doar randam blocurile in functie de starea lor
            if (learning == false) {
                for (int i = 0; i < 10; i++) {
                    for (int j = 0; j < 5; j++) {
                        int& health = blockMatrix[i][j].first;
                        float& scale = blockScale[i][j];
						//starea -1 inseamna ca blocul a fost spart si nu mai trebuie desenat
                        if (health == -1) {
                            continue;
                        }
                        //calculeaza pozitia si dimensiunea originala
                        float bw_original = 1.9f;
                        float bh_original = 0.8f;
                        float bx_original = i * blockDimension - 0.05f + 0.1f;
                        float by_original = logicSpace.height - (j * 0.85f + 2);
						//daca avem daor o viata randam blocul albastru
                        std::string meshToRender = "blueSquare";
                        //daca avem mai mul de o viata
                        if (health > 0) {
                            if (health == 3) 
                                meshToRender = "redSquare";
                            else if (health == 2) 
                                meshToRender = "yellowSquare";
                        }
						//animam spargerea blocului
                        else if (health == 0) {
                            scale -= brickShrinkSpeed * deltaTimeSeconds;
                            if (scale < 0.0f) {
                                scale = 0.0f;
                                health = -1;
                                blockRemaining--;
                                continue;
                            }
							//culoarea blocului devine albastra in timpul animatiei
                            meshToRender = "blueSquare";
                        }

                        // calculeaza scale si translate pentru animatie
                        float bw_scaled = bw_original * scale;
                        float bh_scaled = bh_original * scale;
                        float bx_scaled = bx_original + (bw_original - bw_scaled) / 2.0f;
                        float by_scaled = by_original + (bh_original - bh_scaled) / 2.0f;
						//randam blocul
                        modelMatrix = visMatrix * transform2D::Translate(bx_scaled, by_scaled) * transform2D::Scale(bw_scaled, bh_scaled);
                        RenderMesh2D(meshes[meshToRender], shaders["VertexColor"], modelMatrix);
                    }
                }
            }
			//desenam nava si logica ei
            DrawShip(visMatrix, deltaTimeSeconds);
			//desenam mingea si logica ei
            DrawBall(visMatrix, deltaTimeSeconds);
        }




        /*
         *  These are callback functions. To find more about callbacks and
         *  how they behave, see `input_controller.h`.
         */

        void Tema::OnKeyPress(int key, int mods)
        {
			//cu backspace ne intoarcem la ecranul anterior
            if (key == GLFW_KEY_BACKSPACE)
                if (status > 0)
                    status--;
			//cu space punem pauza jocului
            if (key == GLFW_KEY_P) {
                isPaused = !isPaused;
            }
        }

        void Tema::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
        {
			// logica pentru detectarea mouse-ului peste sageata
            glm::vec3 mousePos = glm::inverse(visMatrix) * glm::vec3(mouseX, resolution.y - mouseY, 1);
            float mouseXlogic = mousePos.x;
            float mouseYlogic = mousePos.y;
			mouse_X = mouseXlogic;
            mouse_Y = mouseYlogic;
            // Coordonatele sagetii
            float arrowWidth = arrowW;
            float arrowHeight = arrowH;
            float arrowLeft = arrowX;
            float arrowRight = arrowX + arrowWidth;
            float arrowBottom = arrowY;
            float arrowTop = arrowY + arrowHeight;
			//daca suntem peste sageata si suntem in ecranul de start, oprim miscarea acesteia
            if (mouseXlogic >= arrowLeft && mouseXlogic <= arrowRight &&
                mouseYlogic >= arrowBottom && mouseYlogic <= arrowTop && status == 1)
            {
                movement = 0;
            }
            else 
			    movement = 1;

        }


        void Tema::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
        {
			//preluam coordonatele mouse-ului in sistemul logic
            glm::vec3 mousePos = glm::inverse(visMatrix) * glm::vec3(mouseX, resolution.y - mouseY, 1);
            float mouseXlogic = mousePos.x;
            float mouseYlogic = mousePos.y;
            if (button == 1 || button == 0) {
				//daca suntem pe ecranul de start
                if (status == 1) {
                    //daca apasam click stanga pe sageata din ecranul de start
                    float arrowWidth = arrowW;
                    float arrowHeight = arrowH;
                    float arrowLeft = arrowX;
                    float arrowRight = arrowX + arrowWidth;
                    float arrowBottom = arrowY;
                    float arrowTop = arrowY + arrowHeight;

                    if (mouseXlogic >= arrowLeft && mouseXlogic <= arrowRight &&
                        mouseYlogic >= arrowBottom && mouseYlogic <= arrowTop)
                    {
                        //ne mutam in ecranul de grid
                        status = 2;
                    }
                }
				//daca suntem in ecranul de grid    
                if (status == 2) {
                    //coordonatele butonului
                    float colorSquareSize = 1.0f;
                    float baseX = 5.2f;
                    float baseY = logicSpace.height - 4.0f;

					//lista cu butoanele de culoare si culorile lor corespunzatoare
                    std::vector<std::pair<std::string, glm::vec3>> colorButtons = {
                        {"redSquare", red},
                        {"blueSquare", blue},
                        {"greenSquare", green},
                        {"yellowSquare", yellow},
                        {"orangeSquare", orange},
                        {"purpleSquare", purple}
                    };
					//verificam ce buton de culoare a fost apasat si modificam culoarea navei
                    for (int i = 0; i < (int)colorButtons.size(); i++) {
						//calculam pozitia butonului
                        float y = baseY - i * 1.1f;
                        float left = baseX;
                        float right = baseX + colorSquareSize;
                        float bottom = y;
                        float top = y + colorSquareSize;

                        if (mouseXlogic >= left && mouseXlogic <= right &&
                            mouseYlogic >= bottom && mouseYlogic <= top) {
							//schimbam culoarea navei
                            shipColour = colorButtons[i].second;
							//refacem mesh-urile navei cu noua culoare
                            Mesh* engine = object2D::CreateEngine("engine", glm::vec3(0, 0, 0), length, shipColour, true);
                            AddMeshToList(engine);

                            Mesh* bumper = object2D::CreateBumper("bumper", glm::vec3(0, 0, 0), length, shipColour, glm::vec3(0.96f, 0.96f, 0.86f), true);
                            AddMeshToList(bumper);

                            Mesh* square = object2D::CreateBlock("square", glm::vec3(0, 0, 0), length, shipColour, true);
                            AddMeshToList(square);
                        }
                    }
                    //verificam daca apasam pe unul din obiectele ce pot fi plasate in grid    
					//patratul
                    float square_right = 3.25f;
                    float square_left = 1.64f;
                    float square_bottom = 6.35f;
                    float square_top = 7.99f;
                    if (mouseXlogic <= square_right && mouseXlogic >= square_left &&
                        mouseYlogic <= square_top && mouseYlogic >= square_bottom) {
                        isDragging = true;
                        todrag = "square";
                        offsetX = mouseXlogic - square_left;
                        offsetY = mouseYlogic - square_bottom;
                    }
					//motorul
                    float engine_bottom = 0.625f;
                    float engine_top = 1.950f;
                    float engine_left = 2.05f;
                    float engine_right = 3;
                    if (mouseXlogic <= engine_right && mouseXlogic >= engine_left &&
                        mouseYlogic <= engine_top && mouseYlogic >= engine_bottom) {
                        isDragging = true;
                        todrag = "engine";
                        offsetX = mouseXlogic - engine_left;
                        offsetY = mouseYlogic - engine_bottom;
                    }
					//tunul
                    float gun_top = 10.95f;
                    float gun_bottom = 8.85f;
                    float gun_left = 2.05f;
                    float gun_right = 2.77f;
                    if (mouseXlogic <= gun_right && mouseXlogic >= gun_left &&
                        mouseYlogic <= gun_top && mouseYlogic >= gun_bottom) {
                        isDragging = true;
                        todrag = "gun";
                        offsetX = mouseXlogic - gun_left;
                        offsetY = mouseYlogic - gun_bottom;
                    }
					//bumper-ul
                    float bumper_bottom = 3.47f;
                    float bumper_top = 4.88f;
                    float bumper_left = 1.42f;
                    float bumper_right = 3.47f;
                    if (mouseXlogic <= bumper_right && mouseXlogic >= bumper_left &&
                        mouseYlogic <= bumper_top && mouseYlogic >= bumper_bottom) {
                        isDragging = true;
                        todrag = "bumper";
                        if (mouseYlogic <= 4.17f)
                            offsetX = mouseXlogic - bumper_left - 0.69f;
                        offsetX = mouseXlogic - bumper_left;
                        offsetY = mouseYlogic - bumper_bottom;
                    }
					//verificam daca apasam pe butonul de start
                    float buttonSize = 1.15f;
                    float buttonLeft = logicSpace.width - 1.7f;
                    float buttonRight = buttonLeft + buttonSize;
                    float buttonBottom = logicSpace.height - 1.7f;
                    float buttonTop = buttonBottom + buttonSize;
					//daca apasam pe buton si culoarea este verde(nava e valida), incepem jocul
                    if (mouseXlogic >= buttonLeft && mouseXlogic <= buttonRight &&
                        mouseYlogic >= buttonBottom && mouseYlogic <= buttonTop)
                    {
                        if (colourButton == green) {
                            status = 3;
                        }
                    }
                }
            }
			//daca suntem in grid si aparam click dreapta, stergem un bloc din grid 
            if (button == 2 && status == 2) {
				//parcurgem gridul pentru a vedea peste ce celula am dat click                
                for (int i = 0; i < 12; i++) {
                    for (int j = 0; j < 8; j++) {
						//ii salvam pozitia
                        float squareX = gridMatrix[i][j].first;
                        float squareY = j * 1.05f + gridStartY;
                        float squareSize = 1.0f;
						int type = gridMatrix[i][j].second;
						//verificam daca mouse-ul este peste celula
                        if (mouseXlogic >= squareX && mouseXlogic <= squareX + squareSize &&
                            mouseYlogic >= squareY && mouseYlogic <= squareY + squareSize) {
							//daca celula nu e goala, o golim
                            gridMatrix[i][j].second = 0;
							//golim si celulele auxiliare in functie de tipul blocului
                            //pentru motor
                            if (type == 1 && gridMatrix[i][j - 1].second == 5)
                                gridMatrix[i][j - 1].second = 0;
							//pentru bumper
                            if (type == 2 && gridMatrix[i][j + 1].second == 5 && gridMatrix[i - 1][j + 1].second == 5 && gridMatrix[i + 1][j + 1].second == 5) {
                                gridMatrix[i][j + 1].second = 0;
                                gridMatrix[i - 1][j + 1].second = 0;
                                gridMatrix[i + 1][j + 1].second = 0;
                            }
							//pentru tun
                            if (type == 4 && gridMatrix[i][j + 1].second == 5 && gridMatrix[i][j + 2].second == 5) {
                                gridMatrix[i][j + 1].second = 0;
                                gridMatrix[i][j + 2].second = 0;
                            }
							//scadem numarul de blocuri plasate
                            numBlocks--;
                        }
                    }
                }
            }
        }

        void Tema::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
        {
			//daca suntem in ecranul de grid si eliberam butonul mouse-ului
            if (status == 2) {
                if (button == 1 || button == 0) {
					//parcugem gridul pentru a vedea unde am eliberat mouse-ul
                    for (int i = 0; i < 12; i++) {
                        for (int j = 0; j < 8; j++) {
                            float squareX = gridMatrix[i][j].first;
                            float squareY = j * 1.05f + gridStartY;
                            float squareSize = 1.0f;
                            //daca dadeam drag unui obiect
                            if (isDragging) {
								//vedem peste ce celula am eliberat mouse-ul
                                if (mouse_X >= squareX && mouse_X <= squareX + squareSize &&
                                    mouse_Y >= squareY && mouse_Y <= squareY + squareSize)
                                {
									//daca celula e goala si nu am depasit numarul maxim de blocuri
                                    if (gridMatrix[i][j].second == 0 && numBlocks < 10)
                                    {
                                        int type = 0;
                                        if (todrag == "engine")
                                            type = 1;
                                        else if (todrag == "bumper")
                                            type = 2;
                                        else if (todrag == "square") 
                                            type = 3;
                                        else if (todrag == "gun") 
                                            type = 4;
										//salvam tipul blocului in celula
                                        gridMatrix[i][j].second = type;
										//populam celulele auxiliare in functie de tipul blocului
										//pentru motor
                                        if (type == 1)
                                            gridMatrix[i][j - 1].second = 5;
										//pentru bumper
                                        if (type == 2) {
                                            gridMatrix[i][j + 1].second = 5;
                                            gridMatrix[i - 1][j + 1].second = 5;
                                            gridMatrix[i + 1][j + 1].second = 5;
                                        }
                                        //pentru tun
                                        if (type == 4) {
                                            gridMatrix[i][j + 1].second = 5;
                                            gridMatrix[i][j + 2].second = 5;
                                        }
										//incepem verificarile de validitate a navei
                                        bool valid = true;

										//MATRICEA NAVEI ESTE INVERSATA PE VERTICALA IN GRID

                                        // peste motor nu poate exista niciun bloc
                                        if (valid) {
											//parcurgem gridul
                                            for (int x = 0; x < 12; x++) {
                                                for (int y = 0; y < 8; y++) {
                                                    //unde gasim motor verificam dedesubtul lui
                                                    if (gridMatrix[x][y].second == 1) {
                                                        for (int yy = y - 2; yy > 0; yy--) {
                                                            if (gridMatrix[x][yy].second != 0) {
                                                                valid = false;
                                                                break;
                                                            }
                                                        }
                                                        if (!valid) break;
                                                    }
                                                }
                                                if (!valid) break;
                                            }
                                        }

                                        // peste tun nu poate exista niciun bloc
                                        if (valid) {
											//parcurgem gridul si unde gasim tun verificam deasupra lui
                                            for (int x = 0; x < 12; x++) {
                                                for (int y = 0; y < 8; y++) {
                                                    if (gridMatrix[x][y].second == 4) {
                                                        for (int yy = y + 3; yy < 8; yy++) {
                                                            if (gridMatrix[x][yy].second != 0) {
                                                                valid = false;
                                                                break;
                                                            }
                                                        }
                                                        if (!valid) break;
                                                    }
                                                }
                                                if (!valid) break;
                                            }
                                        }

										//deasupra bumper-ului nu poate exista niciun bloc
										//pe toate cele 3 pozitii orizontale ale bumper-ului
                                        if (valid) {
                                            for (int x = 0; x < 12; x++) {
                                                for (int y = 0; y < 8; y++) {
                                                    if (gridMatrix[x][y].second == 2) {
														//verificam cele 3 pozitii orizontale
                                                        for (int dx = -1; dx <= 1; dx++) {
                                                            int nx = x + dx;
                                                            if (nx < 0 || nx >= 12) continue;
                                                            for (int yy = y + 2; yy < 8; yy++) {
                                                                if (gridMatrix[nx][yy].second != 0) {
                                                                    valid = false;
                                                                    break;
                                                                }
                                                            }
                                                            if (!valid) break;
                                                        }
                                                        if (!valid) break;
                                                    }
                                                }
                                                if (!valid) break;
                                            }
                                        }

										// nu putem avea un MOTOR langa un TUN (orizontal)
                                        if (valid) {
                                            for (int x = 0; x < 12; x++) {
                                                for (int y = 0; y < 8; y++) {
                                                    if (gridMatrix[x][y].second == 4) {
                                                        //asemanator ca mai sus, 
                                                        for (int dx : { -1, 1 }) {
                                                            int nx = x + dx;
                                                            if (nx >= 0 && nx < 12 && gridMatrix[nx][y].second == 2) {
                                                                valid = false;
                                                                break;
                                                            }
                                                        }
                                                        if (!valid) break;
                                                    }
                                                }
                                                if (!valid) break;
                                            }
                                        }

										// doi bumperi nu pot fi alaturati (orizontal sau vertical)
                                        if (valid) {
                                            for (int x = 0; x < 12; x++) {
                                                for (int y = 0; y < 8; y++) {
                                                    if (gridMatrix[x][y].second == 2) {
														//orizontal
                                                        int dirs[2][2] = { {1,0}, {-1,0} };
                                                        for (auto& d : dirs) {
                                                            int nx = x + d[0], ny = y + d[1];
                                                            if (nx >= 0 && nx < 12 && gridMatrix[nx][ny].second == 2) {
                                                                valid = false;
                                                                break;
                                                            }
                                                        }
                                                        if (!valid) break;
                                                    }
                                                }
                                                if (!valid) break;
                                            }
                                        }

										// maxim 10 blocuri
                                        if (numBlocks > 10) {
                                            valid = false;
                                        }

										// daca e invalid, stergem blocul plasat si cele auxiliare
                                        if (!valid) {
											//celula principala
                                            gridMatrix[i][j].second = 0;
											//pentru motor
                                            if (type == 1)
                                                gridMatrix[i][j - 1].second = 0;
											//pentru bumper
                                            if (type == 2) {
                                                gridMatrix[i][j + 1].second = 0;
                                                gridMatrix[i - 1][j + 1].second = 0;
                                                gridMatrix[i + 1][j + 1].second = 0;
                                            }
                                            //pentru tun
                                            if (type == 4) {
                                                gridMatrix[i][j + 1].second = 0;
                                                gridMatrix[i][j + 2].second = 0;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
				//resetam variabilele de drag
                isDragging = false;
                todrag = "";
            }
        }
