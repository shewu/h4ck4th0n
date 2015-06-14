#include "UnholyGameViewController.h"

#include <algorithm>
#include <cmath>
#include <iostream>

UnholyGameViewController::UnholyGameViewController() : GameViewController() {
    _initGL();
}

UnholyGameViewController::~UnholyGameViewController() {
}

void UnholyGameViewController::process() {
    GameViewController::process();
}

void UnholyGameViewController::_initGL() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, float(WIDTH) / HEIGHT, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
    _quad = gluNewQuadric();
    glClearColor(0.0f, 0.5f, 1.0f, 1.0f);

    GLfloat light_ambient[] = {0.6f, 0.6f, 0.6f, 1.0f};
    GLfloat light_diffuse[] = {0.4f, 0.4f, 0.4f, 1.0f};
    GLfloat light_specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat light_position[] = {1.0f, 1.0f, 1.0f, 0.0f};
    GLfloat mat_ambient[] = {0.2f, 0.2f, 0.2f, 1.0f};
    GLfloat mat_diffuse[] = {0.8f, 0.8f, 0.8f, 1.0f};
    GLfloat mat_specular[] = {0.8f, 0.8f, 0.8f, 0.0f};

    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 100.0);

    glEnable(GL_LIGHTING);  // enable lighting
    glEnable(GL_LIGHT0);    // enable light 0

    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);
}

void UnholyGameViewController::render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_MULTISAMPLE_ARB);

    glLoadIdentity();

    float focusx = 0.0f;
    float focusy = 0.0f;
    if (world->getMyObject() != NULL) {
        focusx = world->getMyObject()->center.x;
        focusy = world->getMyObject()->center.y;
    }
    if (focusx < world->getMinX() + 6)
        focusx = world->getMinX() + 6;
    if (focusx > world->getMaxX() - 6)
        focusx = world->getMaxX() - 6;
    if (focusy < world->getMinY() + 6)
        focusy = world->getMinY() + 6;
    if (focusy > world->getMaxY() - 6)
        focusy = world->getMaxY() - 6;

    gluLookAt(focusx - 6 * cos(angle), focusy - 6 * sin(angle), 3, focusx,
              focusy, 0.0, 0.0, 0.0, 1.0);
    float matrix[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, matrix);

    // fake reflections
    glPushMatrix();
    glScalef(1, 1, -1);
    _drawObjects();
    _drawWalls();
    glPopMatrix();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(1.0, 1.0, 1.0, 1.0);
    _drawFloor(.82f);
    glDisable(GL_BLEND);

    // draw the things right-side up
    _drawObjects();
    _drawWalls();

    glDisable(GL_MULTISAMPLE_ARB);

    glFlush();

    if (mainmenu->isActive()) {
        glDisable(GL_LIGHTING);
        mainmenu->drawMenu();
        glEnable(GL_LIGHTING);
    }
}

void UnholyGameViewController::_drawWalls() {
    glBegin(GL_QUADS);
    for (auto &iter : world->getRectangularWalls()) {
        RectangularWall *wall = iter.second;
        MaterialPtr color = wall->getMaterial();
        glColor3f(color->getR() / 255.0f, color->getG() / 255.0f,
                  color->getB() / 255.0f);
        glVertex3f(wall->p1.x, wall->p1.y, 0);
        glVertex3f(wall->p2.x, wall->p2.y, 0);
        glVertex3f(wall->p2.x, wall->p2.y, 1);
        glVertex3f(wall->p1.x, wall->p1.y, 1);
    }

    for (auto &iter : world->getRoundWalls()) {
        RoundWall *wall = iter.second;
        MaterialPtr color = wall->getMaterial();
        glColor3f(color->getR() / 255.0f, color->getG() / 255.0f,
                  color->getB() / 255.0f);
        float th1 = wall->theta1;
        float th2 = wall->theta2;
        for (float t = th1; t < th2; t += CYL_ANGLE_DELTA) {
            glVertex3f(wall->center.x + wall->radius * std::cos(t),
                       wall->center.y + wall->radius * std::sin(t), 0);
            glVertex3f(
                wall->center.x + wall->radius * std::cos(t + CYL_ANGLE_DELTA),
                wall->center.y + wall->radius * std::sin(t + CYL_ANGLE_DELTA),
                0);
            glVertex3f(
                wall->center.x + wall->radius * std::cos(t + CYL_ANGLE_DELTA),
                wall->center.y + wall->radius * std::sin(t + CYL_ANGLE_DELTA),
                1);
            glVertex3f(wall->center.x + wall->radius * std::cos(t),
                       wall->center.y + wall->radius * std::sin(t), 1);
        }
    }
    glEnd();
}

void UnholyGameViewController::_drawObjects() {
    glEnable(GL_NORMALIZE);
    for (auto &iter : world->getMovingRoundObjects()) {
        MovingRoundObject *obj = iter.second;
        glPushMatrix();
        glTranslatef(obj->center.x, obj->center.y, 0);
        glScalef(obj->radius, obj->radius, obj->heightRatio * obj->radius);
        MaterialPtr color = obj->getMaterial();
        glColor3f(color->getR() / 255.0f, color->getG() / 255.0f,
                  color->getB() / 255.0f);
        gluSphere(_quad, 1.0, 50, 50);
        glPopMatrix();
    }
    glDisable(GL_NORMALIZE);
}

void UnholyGameViewController::_drawFloor(const float alpha_) {
    const float size = 6.0f;
    const float lenX = world->getMaxX() - world->getMinX();
    const float lenY = world->getMaxY() - world->getMinY();
    assert(lenX > 0.f);
    assert(lenY > 0.f);

    glEnable(GL_NORMALIZE);
    glBegin(GL_QUADS);
    for (float x = 0; x < lenX; x += std::min(size, lenX - x)) {
        for (float y = 0; y < lenY; y += std::min(size, lenY - y)) {
            if (int((x + y) / size) % 2 == 1) {
                glColor4f(1.f, 1.f, 1.f, alpha_);
            } else {
                glColor4f(0.f, 0.f, 0.f, alpha_);
            }

            glNormal3f(0, 0, 1);
            glVertex3f(x + world->getMinX(), y + world->getMinY(), 0);
            glVertex3f(x + size + world->getMinX(), y + world->getMinY(), 0);
            glVertex3f(x + size + world->getMinX(), y + size + world->getMinY(),
                       0);
            glVertex3f(x + world->getMinX(), y + size + world->getMinY(), 0);
        }
    }
    glEnd();
    glDisable(GL_NORMALIZE);
}
