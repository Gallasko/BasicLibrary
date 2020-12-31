#include "game.h"

struct Position
{
    float x;
    float y;
    float z;
};

struct Velocity
{
    float x;
    float y;
    float z;
};

GameWindow::GameWindow(QWindow *parent) : QWindow(parent)
{
    setSurfaceType(QWindow::OpenGLSurface);
}

GameWindow::~GameWindow()
{
    delete m_device;
}

void GameWindow::render(QPainter *painter)
{
    Q_UNUSED(painter);
}

void GameWindow::initialize()
{
	initializeOpenGLFunctions();

    std::cout << "Init Ecs" << std::endl;
    EntitySystem ecs;

    std::cout << "Creating 4 Entities: " << std::endl;
    auto ent1 = ecs.createEntity();
    auto ent2 = ecs.createEntity();
    auto ent3 = ecs.createEntity();
    auto ent4 = ecs.createEntity();

    Position pos1 = { 1.5f, 1.0f, 0.0f };
    Position pos2 = { 0.0f, 1.0f, 0.0f };
    Position pos3 = { -1.0f, 1.0f, 0.0f };

    Velocity vel1 = { 0.0f, 0.0f, 0.0f };

    std::cout << "Attaching Position to Entities: " << std::endl;
    ecs.attach<Position>(ent1, pos1);
    ecs.attach<Position>(ent1, pos2);
    ecs.attach<Position>(ent2, pos2);
    ecs.attach<Position>(ent3, pos3);
    ecs.attach<Position>(ent4, pos2);

    ecs.attach<Velocity>(ent1, vel1);

    for(int x = 0; x < 10; x++)
    {
        auto ent = ecs.createEntity();
        ecs.attach<Position>(ent, { 1.5f, 1.0f, 0.0f } );
    }

    std::cout << "Position Attached" << std::endl;

    if(ent1->has<Position>())
        std::cout << "Entity 1 has Position" << std::endl;
    
    if(ent1->has<Velocity>())
        std::cout << "Entity 1 has Velocity" << std::endl;

    //std::cout << ent1.componentList[typeid(Position).name()]->entityId << std::endl;

    auto posView = ecs.view<Position>();

    int i = 0;

    std::cout << "Printing every position" << std::endl;

    for(const auto& pos : posView)
    {
        std::cout << i++ << ": " << pos.x << " " << pos.y << " " << pos.z << std::endl;
    }

    i = 0;

    std::cout << "Updating every position" << std::endl;

    for(auto& pos : posView)
    {
        std::cout << "Before: " << pos.z;
        pos.z = i * 10;
        i++;
        std::cout << " After: " << pos.z << std::endl;
        //std::cout << i++ << ": " << pos.x << " " << pos.y << " " << pos.z << std::endl;
    }

    i = 0;

    std::cout << "Printing every position" << std::endl;

    for(auto pos : posView)
    {
        std::cout << i++ << ": " << pos.x << " " << pos.y << " " << pos.z << std::endl;
    }

    auto velView = ecs.view<Velocity>();

    i = 0;

    std::cout << "Printing every velocity" << std::endl;

    for(auto vel : velView)
    {
        std::cout << i++ << ": " << vel.x << " " << vel.y << " " << vel.z << std::endl;
    }

    auto pos = ent1->get<Position>();
    if(pos != nullptr)
        std::cout << "Position of Entity 1: " << pos->x << " " << pos->y << " " << pos->z << std::endl; 

    pos->x = 5.0f;
    pos->z = -1.0f;

    pos = ent1->get<Position>();
    if(pos != nullptr)
        std::cout << "Position of Entity 1: " << pos->x << " " << pos->y << " " << pos->z << std::endl; 

    std::cout << "Detaching position from Entity 1" << std::endl;

    ecs.dettach<Position>(ent1);

    std::cout << (ent1->has<Position>() ? "Error" : "Done") << std::endl;

    std::cout << "Detaching velocity from Entity 1" << std::endl;

    ecs.dettach<Velocity>(ent1);

    std::cout << (ent1->has<Velocity>() ? "Error" : "Done") << std::endl;

    std::cout << "Detaching position from Entity 2" << std::endl;

    ecs.dettach<Position>(ent2);

    std::cout << (ent2->has<Position>() ? "Error" : "Done") << std::endl;

    auto ent5 = ecs.createEntity();
    ecs.attach<Position>(ent5, { 15.0f, 25.0f, 2.0f } );

    i = 0;

    std::cout << "Printing every position" << std::endl;

    auto posView2 = ecs.view<Position>();

    for(const auto& pos : posView2)
    {
        std::cout << i++ << ": " << pos.x << " " << pos.y << " " << pos.z << std::endl;
    }

    std::cout << "Detaching position from Entity 4" << std::endl;

    ecs.dettach<Position>(ent4);

    std::cout << (ent4->has<Position>() ? "Error" : "Done") << std::endl;

    std::cout << "Detaching position from Entity 5" << std::endl;

    ecs.dettach<Position>(ent5);

    std::cout << (ent5->has<Position>() ? "Error" : "Done") << std::endl;

    i = 0;

    std::cout << "Printing every position" << std::endl;

    auto posView3 = ecs.view<Position>();

    for(const auto& pos : posView3)
    {
        std::cout << i++ << ": " << pos.x << " " << pos.y << " " << pos.z << std::endl;
    }

    i = 0;

    std::cout << "Printing every velocity" << std::endl;

    auto velView2 = ecs.view<Velocity>();

    for(auto vel : velView2)
    {
        std::cout << i++ << ": " << vel.x << " " << vel.y << " " << vel.z << std::endl;
    }

    ecs.attach<Velocity>(ent1, { 1.0f, -1.0f, 0.0f } );
    ecs.attach<Velocity>(ent2, { 5.0f, 0.0f, -5.0f } );

    i = 0;

    std::cout << "Printing every velocity" << std::endl;

    auto velView3 = ecs.view<Velocity>();

    for(auto vel : velView3)
    {
        std::cout << i++ << ": " << vel.x << " " << vel.y << " " << vel.z << std::endl;
    }

    ecs.dettach<Velocity>(ent1);

    i = 0;

    std::cout << "Printing every velocity" << std::endl;

    auto velView4 = ecs.view<Velocity>();

    for(auto vel : velView4)
    {
        std::cout << i++ << ": " << vel.x << " " << vel.y << " " << vel.z << std::endl;
    }

    ecs.dettach<Velocity>(ent2);

    i = 0;

    std::cout << "Printing every velocity" << std::endl;

    auto velView5 = ecs.view<Velocity>();

    for(auto vel : velView5)
    {
        std::cout << i++ << ": " << vel.x << " " << vel.y << " " << vel.z << std::endl;
    }

    ecs.attach<Velocity>(ent1, { 1.0f, -1.0f, 0.0f } );
    ecs.attach<Velocity>(ent2, { 5.0f, 0.0f, -5.0f } );

    i = 0;

    std::cout << "Printing every velocity" << std::endl;

    auto velView6 = ecs.view<Velocity>();

    for(auto vel : velView6)
    {
        std::cout << i++ << ": " << vel.x << " " << vel.y << " " << vel.z << std::endl;
    }

    ecs.dettach<Velocity>(ent2);

    i = 0;

    std::cout << "Printing every velocity" << std::endl;

    auto velView7 = ecs.view<Velocity>();

    for(auto vel : velView7)
    {
        std::cout << i++ << ": " << vel.x << " " << vel.y << " " << vel.z << std::endl;
    }

    ecs.dettach<Velocity>(ent1);

    i = 0;

    std::cout << "Printing every velocity" << std::endl;

    auto velView8 = ecs.view<Velocity>();

    for(auto vel : velView8)
    {
        std::cout << i++ << ": " << vel.x << " " << vel.y << " " << vel.z << std::endl;
    }
    
    ecs.dettach<Velocity>(ent1);
    
    i = 0;

    std::cout << "Printing every velocity" << std::endl;

    auto velView9 = ecs.view<Velocity>();

    for(auto vel : velView9)
    {
        std::cout << i++ << ": " << vel.x << " " << vel.y << " " << vel.z << std::endl;
    }

    //std::cout << i << ": " << (*posView.end()).x << std::endl;

/*
    i = 0;

    for(auto current = posView.begin(); current != posView.end(); current++)
    {
        std::cout << i++ << std::endl;
    }
*/

    //ticking = true;
    //std::thread t (&GameWindow::tick, this);

    //t.detach();
}

void GameWindow::render()
{
    static auto currentTime = QDateTime::currentMSecsSinceEpoch();
    static auto lastTime = QDateTime::currentMSecsSinceEpoch();

    const qreal retinaScale = devicePixelRatio();
    glViewport(0, 0, width() * retinaScale, height() * retinaScale);

    glClearColor(0.1f, 0.3f, 0.7f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    currentTime = QDateTime::currentMSecsSinceEpoch();
  
    lastTime = currentTime;
}

void GameWindow::setAnimating(bool animating)
{
    m_animating = animating;

    if (animating)
        renderLater();
}

void GameWindow::keyPressEvent(QKeyEvent *event)
{
}

void GameWindow::keyReleaseEvent(QKeyEvent *event)
{
}

void GameWindow::mouseMoveEvent(QMouseEvent *event)
{
}

void GameWindow::mousePressEvent(QMouseEvent *event)
{
}

void GameWindow::mouseReleaseEvent(QMouseEvent *event)
{
}

void GameWindow::renderLater()
{
    requestUpdate();
}

void GameWindow::renderNow()
{
    if (!isExposed())
        return;

    bool needsInitialize = false;

    if (!m_context) {
        m_context = new QOpenGLContext(this);
        m_context->setFormat(requestedFormat());
        m_context->create();

        needsInitialize = true;
    }

    m_context->makeCurrent(this);

    if (needsInitialize) {
        initializeOpenGLFunctions();
        initialize();
    }

    render();

    m_context->swapBuffers(this);

    if (m_animating)
        renderLater();
}

bool GameWindow::event(QEvent *event)
{
    switch (event->type()) {
    case QEvent::UpdateRequest:
        renderNow();
        return true;
    default:
        return QWindow::event(event);
    }
}

void GameWindow::exposeEvent(QExposeEvent *event)
{
    Q_UNUSED(event);

    if (isExposed())
        renderNow();
}

void GameWindow::tick()
{
    auto currentTime = QDateTime::currentMSecsSinceEpoch();
    auto lastTime = QDateTime::currentMSecsSinceEpoch();

    while(ticking)
    {
        lastTime = QDateTime::currentMSecsSinceEpoch();



        currentTime = QDateTime::currentMSecsSinceEpoch();
        std::this_thread::sleep_for(std::chrono::milliseconds(40 - (currentTime - lastTime)));
    }
}