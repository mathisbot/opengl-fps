#include "app.h"


// Global variables
// Doesn't change more than one time during execution
static mat4 projection = GLM_MAT4_IDENTITY_INIT;  // Projection matrix


// SDL Initialization flags
static bool SDLInitialized = 0;
static bool mixerInitalized = 0;


#if DEBUG
static void debugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam)
{
    if(id == 131169 || id == 131185 || id == 131218 || id == 131204) return;
    fprintf(stderr, "---------------\nDebug message (%d): %s\n", id, message);
    switch (source)
    {
        case GL_DEBUG_SOURCE_API: fprintf(stderr, "Source: API"); break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM: fprintf(stderr, "Source: Window System"); break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: fprintf(stderr, "Source: Shader Compiler"); break;
        case GL_DEBUG_SOURCE_THIRD_PARTY: fprintf(stderr, "Source: Third Party"); break;
        case GL_DEBUG_SOURCE_APPLICATION: fprintf(stderr, "Source: Application"); break;
        case GL_DEBUG_SOURCE_OTHER: fprintf(stderr, "Source: Other"); break;
    }
    fprintf(stderr, "\n");
    switch (type)
    {
        case GL_DEBUG_TYPE_ERROR: fprintf(stderr, "Type: Error"); break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: fprintf(stderr, "Type: Deprecated Behaviour"); break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: fprintf(stderr, "Type: Undefined Behaviour"); break;
        case GL_DEBUG_TYPE_PORTABILITY: fprintf(stderr, "Type: Portability"); break;
        case GL_DEBUG_TYPE_PERFORMANCE: fprintf(stderr, "Type: Performance"); break;
        case GL_DEBUG_TYPE_MARKER: fprintf(stderr, "Type: Marker"); break;
        case GL_DEBUG_TYPE_PUSH_GROUP: fprintf(stderr, "Type: Push Group"); break;
        case GL_DEBUG_TYPE_POP_GROUP: fprintf(stderr, "Type: Pop Group"); break;
        case GL_DEBUG_TYPE_OTHER: fprintf(stderr, "Type: Other"); break;
    }
    fprintf(stderr, "\n");
    switch (severity)
    {
        case GL_DEBUG_SEVERITY_HIGH: fprintf(stderr, "Severity: high"); break;
        case GL_DEBUG_SEVERITY_MEDIUM: fprintf(stderr, "Severity: medium"); break;
        case GL_DEBUG_SEVERITY_LOW: fprintf(stderr, "Severity: low"); break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: fprintf(stderr, "Severity: notification"); break;
    }
    fprintf(stderr, "\n\n");
}
#endif


static void appCleanUp(Application* app)
{
    // SDL
    if (app->glContext) {SDL_GL_DeleteContext(app->glContext); app->glContext = NULL;}
    if (app->window) {SDL_DestroyWindow(app->window); app->window = NULL;}
    if (SDLInitialized) {SDL_Quit(); SDLInitialized = 0;}
    if (mixerInitalized) {Mix_CloseAudio(); Mix_Quit(); mixerInitalized = 0;}

    // OpenGL
    if (app->VBO) {glDeleteBuffers(1, &app->VBO); app->VBO = 0;}
    if (app->uiVBO) {glDeleteBuffers(1, &app->uiVBO); app->uiVBO = 0;}

    if (app->depthMapFBO) {glDeleteFramebuffers(1, &app->depthMapFBO); app->depthMapFBO = 0;}

    if (app->lightVAO) {glDeleteVertexArrays(1, &app->lightVAO); app->lightVAO = 0;}
    if (app->uiVAO) {glDeleteVertexArrays(1, &app->uiVAO); app->uiVAO = 0;}

    if (app->shaderProgram) {glDeleteProgram(app->shaderProgram); app->shaderProgram = 0;}
    if (app->shaderProgramUI) {glDeleteProgram(app->shaderProgramUI); app->shaderProgramUI = 0;}
    if (app->shaderProgramLight) {glDeleteProgram(app->shaderProgramLight); app->shaderProgramLight = 0;}
    if (app->shaderProgramDepth) {glDeleteProgram(app->shaderProgramDepth); app->shaderProgramDepth = 0;}

    // Freeing other components
    for (uint8_t i=0; i<sizeof(app->pointLights)/sizeof(PointLight); i++) destroyPointLight(&app->pointLights[i]);
    if (app->scene.loaded) destroyScene(&app->scene);

    LOG_INFO("Application cleaned up\n");
}

static void appCleanUpAndExit(Application* app, bool exitCode, const char* log, ...)
{
    va_list args;
    va_start(args, log);
    LOG_ERROR(log, args);
    va_end(args);

    appCleanUp(app);
    exit(exitCode);
}


static void appInitScene(Application *app)
{
    // Point lights
    if (initPointLight(&app->pointLights[0], (vec3){0.0f, 2.0f, 2.0f}, (vec3){1.0f, 1.0f, 1.0f})<0) appCleanUpAndExit(app, EXIT_FAILURE, "Error creating point light");
    if (initPointLight(&app->pointLights[1], (vec3){2.3f, 3.3f, -4.0f}, (vec3){1.0f, 0.0f, 0.0f})<0) appCleanUpAndExit(app, EXIT_FAILURE, "Error creating point light");
    if (initPointLight(&app->pointLights[2], (vec3){-4.0f, 2.0f, -12.0f}, (vec3){0.0f, 1.0f, 0.0f})<0) appCleanUpAndExit(app, EXIT_FAILURE, "Error creating point light");
    if (initPointLight(&app->pointLights[3], (vec3){3.3f, 4.0f, -1.5f}, (vec3){0.0f, 0.0f, 1.0f})<0) appCleanUpAndExit(app, EXIT_FAILURE, "Error creating point light");

    // Load models
    app->scene.modelCount = 2;
    app->scene.models = malloc(sizeof(Model) * app->scene.modelCount);
    if (loadModel(&app->scene.models[0], "guitar/backpack.obj", (vec3){3.0, 1.0, 3.0}, (vec3){1.0, 1.0, 1.0}, false) < 0) appCleanUpAndExit(app, EXIT_FAILURE, "Error loading guitar model");
    if (loadModel(&app->scene.models[1], "shotgun/shotgun.obj", (vec3){2.0, 1.5, 2.0}, (vec3){2.0, 2.0, 2.0}, true) < 0) appCleanUpAndExit(app, EXIT_FAILURE, "Error loading shotgun model");
    // if (loadModel(&app->scene.models[2], "medievalhouse/house.obj", (vec3){15.0, 0.0, 15.0}, (vec3){2.0, 2.0, 2.0}, true) < 0) appCleanUpAndExit(app, EXIT_FAILURE, "Error loading house model");

    // Load skybox
    if (loadSkybox(&app->scene, "skybox/") < 0) appCleanUpAndExit(app, EXIT_FAILURE, "Error loading skybox\n");

    // Vertices for a cube (Temporary lights)
    float vertices[] = {
        -0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,

        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,

        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,

         0.5f,  0.5f,  0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,

        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f,  0.5f, -0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f,  0.5f
    };

    // Bind data to buffers

    // Light
    glBindVertexArray(app->lightVAO);

    glBindBuffer(GL_ARRAY_BUFFER, app->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // UI
    float uiVertices[] = {
        -1.0f, -1.0f, 0.0f,  // Bottom left
         1.0f,  1.0f, 0.0f,  // Top right
        -1.0f,  1.0f, 0.0f,  // Top left
        -1.0f, -1.0f, 0.0f,  // Bottom left
         1.0f, -1.0f, 0.0f,  // Bottom right
         1.0f,  1.0f, 0.0f,  // Top right
    };

    glBindVertexArray(app->uiVAO);

    glBindBuffer(GL_ARRAY_BUFFER, app->uiVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(uiVertices), uiVertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);


    // Unbinding buffers
    glBindVertexArray(0);

    app->scene.loaded = 1;
}

static void appFirstPass(Application *app)
{
    // Projection matrix only needs to be calculated once
    glm_perspective(glm_rad(FOV), (float)app->windowWidth / (float)app->windowHeight, ZNEAR, ZFAR, projection);

    // Light shader
    glUseProgram(app->shaderProgramLight);
    glUniformMatrix4fv(glGetUniformLocation(app->shaderProgramLight, "projection"), 1, GL_FALSE, (float*)projection);
    glUniform2ui(glGetUniformLocation(app->shaderProgramLight, "windowSize"), app->windowWidth, app->windowHeight);
    glUniform1f(glGetUniformLocation(app->shaderProgramLight, "pointerRadius"), 2.0f);

    // UI shader
    glUseProgram(app->shaderProgramUI);
    glUniform2ui(glGetUniformLocation(app->shaderProgramUI, "windowSize"), app->windowWidth, app->windowHeight);
    glUniform1f(glGetUniformLocation(app->shaderProgramUI, "pointerRadius"), 2.0f);

    // Object shader
    glUseProgram(app->shaderProgram);
    glUniformMatrix4fv(glGetUniformLocation(app->shaderProgram, "projection"), 1, GL_FALSE, (float*)projection);
    glUniform2ui(glGetUniformLocation(app->shaderProgram, "windowSize"), app->windowWidth, app->windowHeight);
    glUniform1f(glGetUniformLocation(app->shaderProgram, "pointerRadius"), 2.0f);
    glUniform1f(glGetUniformLocation(app->shaderProgram, "FarPlaneShadow"), SHADOWMAP_ZFAR);
    static const int textureStart = 1;  // Save texture unit 0 just in case
    char locate[32];
    for (uint8_t i=0; i<4; i++)
    {
        sprintf(locate, "pointLights[%d].position", i);
        glUniform3f(glGetUniformLocation(app->shaderProgram, locate), app->pointLights[i].position[0], app->pointLights[i].position[1], app->pointLights[i].position[2]);
        sprintf(locate, "pointLights[%d].ambient", i);
        glUniform3f(glGetUniformLocation(app->shaderProgram, locate), 0.03f, 0.03f, 0.03f);
        sprintf(locate, "pointLights[%d].diffuse", i);
        glUniform3f(glGetUniformLocation(app->shaderProgram, locate), 0.4f, 0.4f, 0.4f);
        sprintf(locate, "pointLights[%d].specular", i);
        glUniform3f(glGetUniformLocation(app->shaderProgram, locate), 1.0f, 1.0f, 1.0f);
        sprintf(locate, "pointLights[%d].linear", i);
        glUniform1f(glGetUniformLocation(app->shaderProgram, locate), 0.09f);
        sprintf(locate, "pointLights[%d].quadratic", i);
        glUniform1f(glGetUniformLocation(app->shaderProgram, locate), 0.032f);
        sprintf(locate, "pointLights[%d].color", i);
        glUniform3f(glGetUniformLocation(app->shaderProgram, locate), app->pointLights[i].color[0], app->pointLights[i].color[1], app->pointLights[i].color[2]);
        sprintf(locate, "pointLights[%d].depthCubemap", i);
        glActiveTexture(GL_TEXTURE0+i+textureStart);
        glBindTexture(GL_TEXTURE_CUBE_MAP, app->pointLights[i].depthCubemap);
        glUniform1i(glGetUniformLocation(app->shaderProgram, locate), i+textureStart);
    }
    glUniform1f(glGetUniformLocation(app->shaderProgram, "material.shininess"), 64.0f);
}

static void appInit(Application* app)
{
    // SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) appCleanUpAndExit(app, 1, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    SDLInitialized = 1;
    LOG_TRACE("SDL initialized\n");

    // Application
    app->quit = 0;
    app->pause = 0;
    app->dt = 0.0f;
    app->keyboardState = SDL_GetKeyboardState(NULL);


    // Handling custom resolution when fullscreen
    #if FULLSCREEN
    SDL_DisplayMode displayMode;
    if (SDL_GetCurrentDisplayMode(0, &displayMode))
        appCleanUpAndExit(app, EXIT_FAILURE, "Error getting display mode : %s", SDL_GetError());
    app->windowWidth = displayMode.w;
    app->windowHeight = displayMode.h;
    #else
    app->windowWidth = 1280; app->windowHeight = 720;
    #endif
    LOG_TRACE("Window size set to %dx%d\n", app->windowWidth, app->windowHeight);


    // OpenGL attributes
    // OpenGL version 4.6.0 Core profile
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);

    #if DEBUG
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);
    #endif
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, MSAADEPTH);


    // Window creation
    app->window = SDL_CreateWindow("FPS", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, app->windowWidth, app->windowHeight, SDL_WINDOW_OPENGL);
    if (!app->window) appCleanUpAndExit(app, EXIT_FAILURE, "Window could not be created! SDL_Error: %s\n", SDL_GetError());
    SDL_ShowCursor(SDL_DISABLE);
    // Handling fullscreen
    SDL_SetWindowFullscreen(app->window, FULLSCREEN ? SDL_WINDOW_FULLSCREEN : 0);
    LOG_TRACE("Window created\n");


    // OpenGL context creation
    app->glContext = SDL_GL_CreateContext(app->window);
    if (!app->glContext) appCleanUpAndExit(app, EXIT_FAILURE, "OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError());
    LOG_TRACE("OpenGL context created\n");

    // GLEW initialization
    GLenum glewInitState = glewInit();
    if (glewInitState != GLEW_OK) appCleanUpAndExit(app, EXIT_FAILURE, "Error initialising GLEW : %s", glewGetErrorString(glewInitState));
    LOG_TRACE("GLEW initialized\n");


    // OpenGL settings
    #if DEBUG
    GLint flags; glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
    {
        LOG_DEBUG("[GL Debug Output enabled]\n");
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(debugCallback, NULL);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
    }
    else LOG_ERROR("[GL Debug Output failed to activate]");
    LOG_DEBUG("> OpenGL version : %s\n", glGetString(GL_VERSION));
    LOG_DEBUG("> OpenGL vendor : %s\n", glGetString(GL_VENDOR));
    LOG_DEBUG("> OpenGL renderer : %s\n", glGetString(GL_RENDERER));
    LOG_DEBUG("> OpenGL shading language version : %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
    LOG_DEBUG("> Vertex shader max attribribute count : %d\n", GL_MAX_VERTEX_ATTRIBS);
    #endif

    if (VSYNC) {if (SDL_GL_SetSwapInterval(-1) == -1) SDL_GL_SetSwapInterval(1);}
    if (WIREFRAME) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_CULL_FACE);  // Triangles have to be defined in counter-clockwise order
    glEnable(GL_FRAMEBUFFER_SRGB);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    
    // Load SDL Mixer
    if (initMixer(AUDIO_NUMCHANS) < 0) appCleanUpAndExit(app, EXIT_FAILURE, "Error initialising Mixer : %s", Mix_GetError());
    mixerInitalized = 1;

    // OpenGL Buffer creation
    glGenBuffers(1, &app->VBO);
    glGenBuffers(1, &app->uiVBO);

    glGenFramebuffers(1, &app->depthMapFBO);
    
    glGenVertexArrays(1, &app->lightVAO);
    glGenVertexArrays(1, &app->uiVAO);

    // OpenGL Shader creation
    Shader vertexShader, vertexShaderUI, vertexShaderDepth, geometryShaderDepth, fragmentShader, fragmentShaderUI, fragmentShaderLight, fragmentShaderDepth;
    if (loadShader(&vertexShader, "vertex.vert", GL_VERTEX_SHADER) < 0) appCleanUpAndExit(app, EXIT_FAILURE, "Error creating vertex shader");
    if (loadShader(&vertexShaderUI, "ui.vert", GL_VERTEX_SHADER) < 0) appCleanUpAndExit(app, EXIT_FAILURE, "Error creating vertex shader for UI");
    if (loadShader(&vertexShaderDepth, "depth.vert", GL_VERTEX_SHADER) < 0) appCleanUpAndExit(app, EXIT_FAILURE, "Error creating vertex shader for depth map");
    if (loadShader(&geometryShaderDepth, "depth.geom", GL_GEOMETRY_SHADER) < 0) appCleanUpAndExit(app, EXIT_FAILURE, "Error creating geometry shader for depth map");
    if (loadShader(&fragmentShader, "fragment.frag", GL_FRAGMENT_SHADER) < 0) appCleanUpAndExit(app, EXIT_FAILURE, "Error creating fragment shader");
    if (loadShader(&fragmentShaderUI, "ui.frag", GL_FRAGMENT_SHADER) < 0) appCleanUpAndExit(app, EXIT_FAILURE, "Error creating fragment shader for UI");
    if (loadShader(&fragmentShaderLight, "light.frag", GL_FRAGMENT_SHADER) < 0) appCleanUpAndExit(app, EXIT_FAILURE, "Error creating fragment shader for light");
    if (loadShader(&fragmentShaderDepth, "depth.frag", GL_FRAGMENT_SHADER) < 0) appCleanUpAndExit(app, EXIT_FAILURE, "Error creating fragment shader for depth map");

    // If program crashes here, there's a memory leak (shaders are not freed)
    // This is done on purpose as they are only used for the next few lines

    // Shader programs
    if (initShaderProgram(&app->shaderProgram, 2, vertexShader, fragmentShader) < 0) appCleanUpAndExit(app, EXIT_FAILURE, "Error creating shader program");
    if (initShaderProgram(&app->shaderProgramLight, 2, vertexShader, fragmentShaderLight) < 0) appCleanUpAndExit(app, EXIT_FAILURE, "Error creating shader program for light");
    if (initShaderProgram(&app->shaderProgramUI, 2, vertexShaderUI, fragmentShaderUI) < 0) appCleanUpAndExit(app, EXIT_FAILURE, "Error creating shader program for UI");
    if (initShaderProgram(&app->shaderProgramDepth, 3, vertexShaderDepth, geometryShaderDepth, fragmentShaderDepth) < 0) appCleanUpAndExit(app, EXIT_FAILURE, "Error creating shader program for depth map");
    
    // Delete now useless shaders
    destroyShader(&vertexShader);
    destroyShader(&vertexShaderUI);
    destroyShader(&vertexShaderDepth);
    destroyShader(&fragmentShader);
    destroyShader(&fragmentShaderUI);
    destroyShader(&fragmentShaderLight);
    destroyShader(&fragmentShaderDepth);


    /* --- Load game objects --- */

    // Camera
    Bindings bindings = {
        SDL_SCANCODE_H,  // Forward
        SDL_SCANCODE_B,  // Backward
        SDL_SCANCODE_V,  // Left
        SDL_SCANCODE_N,  // Right
        SDL_SCANCODE_G,  // Sprint
        SDL_SCANCODE_SPACE,  // Jump
        SDL_SCANCODE_U,  // Use
        SDL_SCANCODE_I  // Reload
    };
    initCamera(&app->camera, (vec3){-2.0f, EYE_Y, 2.0f}, (vec3){-1.0f, EYE_Y, 2.0f}, &bindings);

    // Scene
    appInitScene(app);

    // First render pass
    appFirstPass(app);
}


static void appHandleEvents(Application* app)
{
    static SDL_Event e;
    while (SDL_PollEvent(&e))
        {
            switch (e.type)
            {
                // Quit program
                case SDL_QUIT:
                    app->quit = 1;
                    break;
                // Handle key presses not related to camera
                case SDL_KEYDOWN:
                    switch (e.key.keysym.scancode)
                    {
                        case SDL_SCANCODE_F1:
                            app->quit = 1;
                            break;
                        case SDL_SCANCODE_ESCAPE:
                            app->pause = !app->pause;
                            SDL_ShowCursor(app->pause ? SDL_ENABLE : SDL_DISABLE);
                            break;
                        default:
                            break;
                    }
                    break;
                // Rotate camera
                case SDL_MOUSEMOTION:
                    // Power saving
                    if ((e.motion.xrel == 0 && e.motion.yrel == 0) || app->pause)
                        break;
                    rotateCamera(&app->camera, e.motion.xrel, e.motion.yrel);
                    SDL_WarpMouseInWindow(app->window, app->windowWidth/2, app->windowHeight/2);
                    break;
                // Shoot
                case SDL_MOUSEBUTTONDOWN:
                    if (e.button.button == SDL_BUTTON_LEFT && !app->pause)
                        // TODO : Shoot
                        printf("Shoot !\n");
                    break;
                default:
                    break;
            }
        }
        if (!app->pause)
        {
            translateCamera(&app->camera, app->keyboardState, app->dt);
            updateCamera(&app->camera);
        }
}

static bool appUpdate(Application* app)
{
    static Uint64 lastFrameTime = 0;
    static Uint64 currentFrameTime;
    static Uint64 deltaTimeMS;

    // Time
    currentFrameTime = SDL_GetTicks64();
    deltaTimeMS = currentFrameTime - lastFrameTime;
    if (deltaTimeMS <2) {SDL_Delay(2-deltaTimeMS); return 1;}  // Skip frame if too early
    app->dt = deltaTimeMS / 1000.0f;
    lastFrameTime = currentFrameTime;
    #if PRINT_FPS
    LOG_INFO("FPS : %lf\n", 1.0 / app->dt)
    #endif

    // Events
    appHandleEvents(app);

    if (!app->pause)
    {
        // TODO: Game logic
    }

    return 0;
}


static void appRender(Application* app)
{
    /* --- RENDER ON DEPTH MAP --- */

    renderPointLightsShadowMap(&app->scene, app->shaderProgramDepth, app->depthMapFBO, app->pointLights);


    /* --- RENDER ON SCREEN --- */

    // Clear screen
    glViewport(0, 0, app->windowWidth, app->windowHeight);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // View matrix
    static mat4 view = GLM_MAT4_IDENTITY_INIT;
    glm_lookat(app->camera.pos, app->camera.target, app->camera.up, view);


    /* --- User Interface --- */

    // TODO: Optimize UI rendering
    glUseProgram(app->shaderProgramUI);
    glBindVertexArray(app->uiVAO);

    // Simple quad for now
    glDrawArrays(GL_TRIANGLES, 0, 6);


    /* --- Light sources --- */

    // TODO: Replace cubes by models (e.g. lamps)
    // Currently,there are cubes that use a different shader so they are not affected by lighting and are always visible

    // Use light shader
    glUseProgram(app->shaderProgramLight);

    // Send to shader
    glUniformMatrix4fv(glGetUniformLocation(app->shaderProgramLight, "view"), 1, GL_FALSE, (float*)view);

    // Rendering
    glBindVertexArray(app->lightVAO);
    
    for (uint8_t i=0; i<4; i++)
    {
        glUniform3f(glGetUniformLocation(app->shaderProgramLight, "lightColor"), app->pointLights[i].color[0], app->pointLights[i].color[1], app->pointLights[i].color[2]);

        // Model matrix
        static mat4 modelLight = GLM_MAT4_IDENTITY_INIT;
        glm_mat4_identity(modelLight);
        glm_translate(modelLight, app->pointLights[i].position);
        glm_scale(modelLight, (vec3){0.2f, 0.2f, 0.2f});

        glUniformMatrix4fv(glGetUniformLocation(app->shaderProgramLight, "model"), 1, GL_FALSE, (float*)modelLight);

        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    glBindVertexArray(0);


    /* --- Objects --- */

    glUseProgram(app->shaderProgram);

    // Send to shader
    glUniformMatrix4fv(glGetUniformLocation(app->shaderProgram, "view"), 1, GL_FALSE, (float*)view);
    glUniform3f(glGetUniformLocation(app->shaderProgram, "viewPos"), app->camera.pos[0], app->camera.pos[1], app->camera.pos[2]);

    // Rendering
    renderScene(&app->scene, app->shaderProgram);

    // Swap buffers
    SDL_GL_SwapWindow(app->window);
}


int appRun(Application* app)
{
    appInit(app);
    LOG_DEBUG("Application initialized\n");

    while (!app->quit)
    {
        if (appUpdate(app)) continue;
        appRender(app);
    }

    appCleanUp(app);
    return 0;
}