#include <QGuiApplication>

#include <Qt3DCore/QEntity>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QCameraLens>
#include <Qt3DCore/QTransform>
#include <Qt3DCore/QAspectEngine>

#include <Qt3DInput/QInputAspect>

#include <Qt3DRender/QRenderAspect>
#include <Qt3DRender/QMaterial>
#include <Qt3DRender/QParameter>

#include <Qt3DExtras/QForwardRenderer>
#include <Qt3DExtras/QCylinderMesh>
#include <Qt3DExtras/QSphereMesh>
#include <Qt3DExtras/QTorusMesh>

#include <QGeometryRenderer>

#include <QEffect>

#include <QTechnique>

#include <QRenderPass>

#include <QGraphicsApiFilter>
#include <QUrl>

#include <QShaderProgram>

#include "qt3dwindow.h"
#include "qorbitcameracontroller.h"

#include "instancedgeometry.h"

Qt3DCore::QEntity *createScene()
{
    // Root entity
    Qt3DCore::QEntity *rootEntity = new Qt3DCore::QEntity;

    QMatrix4x4 instTransform = QMatrix4x4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1);

    QVector<QVector3D> pos;
    pos << QVector3D(1, 1, 0);
    pos << QVector3D(-1, 2, 8);
    pos << QVector3D(1, 1, 7);
    pos << QVector3D(0, 0, 4);
    pos << QVector3D(1, 5, 1);
    pos << QVector3D(-3, 3, 0);
    pos << QVector3D(2, 2, -2);

    InstancedGeometry *instGeom;
    instGeom->setPoints(pos);

    // Stars
    Qt3DCore::QEntity *starsInstanced = new Qt3DCore::QEntity(rootEntity);
    
    Qt3DRender::QGeometryRenderer *sphereGeometryRenderer = new Qt3DRender::QGeometryRenderer(starsInstanced);
    sphereGeometryRenderer->setGeometry(instGeom);
    sphereGeometryRenderer->setInstanceCount(instGeom->count());

    Qt3DCore::QTransform *sphereTransform = new Qt3DCore::QTransform;

    Qt3DRender::QParameter *ka = new Qt3DRender::QParameter("ka", QColor::fromRgbF(0.05f, 0.05f, 0.05f, 1.0f));
    Qt3DRender::QParameter *kd = new Qt3DRender::QParameter("kd", QColor::fromRgbF(0.05f, 0.05f, 0.05f, 1.0f));
    Qt3DRender::QParameter *ks = new Qt3DRender::QParameter("ks", QColor::fromRgbF(0.05f, 0.05f, 0.05f, 1.0f));
    Qt3DRender::QParameter *shininess = new Qt3DRender::QParameter("shininess", 150);
    Qt3DRender::QParameter *inst = new Qt3DRender::QParameter("inst", instTransform);
    Qt3DRender::QParameter *instNormal = new Qt3DRender::QParameter("instNormal", instGeom->normalMatrix(instTransform));

    Qt3DRender::QMaterial *material = new Qt3DRender::QMaterial();
    material->addParameter(ka);
    material->addParameter(kd);
    material->addParameter(ks);
    material->addParameter(shininess);
    material->addParameter(inst);
    material->addParameter(instNormal);

    Qt3DRender::QShaderProgram *glShader = new Qt3DRender::QShaderProgram();
    glShader->setVertexShaderCode(Qt3DRender::QShaderProgram::loadSource(QUrl::fromLocalFile("./src/shaders/instanced.vert")));
    glShader->setFragmentShaderCode(Qt3DRender::QShaderProgram::loadSource(QUrl::fromLocalFile("./src/shaders/instanced.frag")));

    Qt3DRender::QRenderPass *pass = new Qt3DRender::QRenderPass();
    pass->setShaderProgram(glShader);

    Qt3DRender::QTechnique *technique = new Qt3DRender::QTechnique();
    technique->graphicsApiFilter()->setApi(Qt3DRender::QGraphicsApiFilter::OpenGL);
    technique->graphicsApiFilter()->setMajorVersion(3);
    technique->graphicsApiFilter()->setMinorVersion(1);
    
    technique->graphicsApiFilter()->setProfile(Qt3DRender::QGraphicsApiFilter::CoreProfile);
    technique->addRenderPass(pass);

    Qt3DRender::QEffect *effect = new Qt3DRender::QEffect();
    effect->addTechnique(technique);

    material->setEffect(effect);

    starsInstanced->addComponent(material);
    starsInstanced->addComponent(sphereTransform);
    starsInstanced->addComponent(sphereGeometryRenderer);

    return rootEntity;
}

int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);
    Qt3DExtras::Qt3DWindow view;

    Qt3DCore::QEntity *scene = createScene();

    // Camera
    Qt3DRender::QCamera *camera = view.camera();
    camera->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.1f, 1000.0f);
    camera->setPosition(QVector3D(0, 0, 40.0f));
    camera->setViewCenter(QVector3D(0, 0, 0));

    // For camera controls
    Qt3DExtras::QOrbitCameraController *camController = new Qt3DExtras::QOrbitCameraController(scene);
    camController->setLinearSpeed( 50.0f );
    camController->setLookSpeed( 180.0f );
    camController->setCamera(camera);

    view.setRootEntity(scene);
    view.show();

    return app.exec();
}
