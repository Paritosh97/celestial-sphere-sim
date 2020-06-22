#include <Qt3DQuickExtras/qt3dquickwindow.h>

#include <QGuiApplication>

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DCore/QAspectEngine>

#include <Qt3DRender/QRenderAspect>
#include <Qt3DRender/QRenderSurfaceSelector>
#include <Qt3DRender/QViewport>
#include <Qt3DRender/QCameraSelector>
#include <Qt3DRender/QClearBuffers>
#include <Qt3DRender/QRenderSettings>
#include <Qt3DRender/QMaterial>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QCameraLens>
#include <Qt3DRender/QParameter>
#include <Qt3DRender/QGeometryRenderer>
#include <Qt3DRender/QEffect>
#include <Qt3DRender/QTechnique>
#include <Qt3DRender/QRenderPass>
#include <Qt3DRender/QGraphicsApiFilter>
#include <Qt3DRender/QShaderProgram>

#include <Qt3DExtras/QForwardRenderer>
#include <Qt3DExtras/QPlaneMesh>
#include <Qt3DExtras/QSphereMesh>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QTorusMesh>
#include <Qt3DExtras/QSkyboxEntity>
#include <Qt3DExtras/QOrbitCameraController>

#include <QUrl>

#include "qt3dwindow.h"

#include "instancedgeometry.h"

#include "skypoint.h"
#include "linelist.h"
#include "typedef.h"

class SkyPoint;

Qt3DCore::QEntity* addSkybox()
{
    Qt3DExtras::QSkyboxEntity* skybox = new Qt3DExtras::QSkyboxEntity;
    QString path = QUrl::fromLocalFile("/home/paritosh/Desktop/gsoc/celestial-sphere-sim/skybox/park").toString();
    skybox->setBaseName(path);
    skybox->setExtension(".png");

    return skybox;
}

Qt3DCore::QEntity *createScene()
{
    // Root entity
    Qt3DCore::QEntity *spheres = new Qt3DCore::QEntity;

    QMatrix4x4 instTransform = QMatrix4x4(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1);

    QVector<QVector3D> pos;
    for(int i = 1; i < 10; i++)
    {
        dms *ra = new dms(i*36);
        dms *dec = new dms(i*30);
        std::shared_ptr<SkyPoint>  point(new SkyPoint(*ra, *dec));

        pos.append(QVector3D(point->ra().radians(), point->dec().radians(), 0));
    }
    qDebug()<<pos;

    InstancedGeometry instGeom;
    instGeom.setPoints(pos);
    
    Qt3DRender::QGeometryRenderer *sphereGeometryRenderer = new Qt3DRender::QGeometryRenderer(spheres);
    sphereGeometryRenderer->setGeometry(&instGeom);
    sphereGeometryRenderer->setInstanceCount(instGeom.count());

    Qt3DCore::QTransform *sphereTransform = new Qt3DCore::QTransform(spheres);

    Qt3DRender::QParameter *ka = new Qt3DRender::QParameter("ka", QColor::fromRgbF(0.05f, 0.05f, 0.05f, 1.0f));
    Qt3DRender::QParameter *kd = new Qt3DRender::QParameter("kd", QColor::fromRgbF(0.7f, 0.7f, 0.7f, 1.0f));
    Qt3DRender::QParameter *ks = new Qt3DRender::QParameter("ks", QColor::fromRgbF(0.01f, 0.01f, 0.01f, 1.0f));
    Qt3DRender::QParameter *shininess = new Qt3DRender::QParameter("shininess", 150);
    Qt3DRender::QParameter *inst = new Qt3DRender::QParameter("inst", instTransform);
    Qt3DRender::QParameter *instNormal = new Qt3DRender::QParameter("instNormal", instGeom.normalMatrix(instTransform));

    Qt3DRender::QMaterial *material = new Qt3DRender::QMaterial(spheres);
    material->addParameter(ka);
    material->addParameter(kd);
    material->addParameter(ks);
    material->addParameter(shininess);
    material->addParameter(inst);
    material->addParameter(instNormal);

    Qt3DRender::QEffect *effect = new Qt3DRender::QEffect(material);
    Qt3DRender::QTechnique *technique = new Qt3DRender::QTechnique(effect);
    Qt3DRender::QRenderPass *pass = new Qt3DRender::QRenderPass(technique);
    Qt3DRender::QShaderProgram *glShader = new Qt3DRender::QShaderProgram(pass);

    glShader->setVertexShaderCode(Qt3DRender::QShaderProgram::loadSource(QUrl::fromLocalFile("/home/paritosh/Desktop/gsoc/celestial-sphere-sim/src/instanced.vert")));
    glShader->setFragmentShaderCode(Qt3DRender::QShaderProgram::loadSource(QUrl::fromLocalFile("/home/paritosh/Desktop/gsoc/celestial-sphere-sim/src/instanced.frag")));

    pass->setShaderProgram(glShader);

    technique->graphicsApiFilter()->setApi(Qt3DRender::QGraphicsApiFilter::OpenGL);
    technique->graphicsApiFilter()->setMajorVersion(3);
    technique->graphicsApiFilter()->setMinorVersion(1);    
    technique->graphicsApiFilter()->setProfile(Qt3DRender::QGraphicsApiFilter::CoreProfile);
    technique->addRenderPass(pass);

    effect->addTechnique(technique);

    material->setEffect(effect);

    spheres->addComponent(material);
    spheres->addComponent(sphereTransform);
    spheres->addComponent(sphereGeometryRenderer);    

    return spheres;
}

int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);

    Qt3DExtras::Qt3DWindow view;

    Qt3DCore::QEntity *root = new Qt3DCore::QEntity();

    // Camera
    Qt3DRender::QCamera *camera = view.camera();
    camera->lens()->setPerspectiveProjection(60.0f, 16.0f/9.0f, 0.01f, 1000.0f);
    camera->setPosition(QVector3D(0, 10, 20));
    camera->setViewCenter(QVector3D(0, 0, 0));
    camera->setUpVector(QVector3D(0, 1, 0));
    camera->setAspectRatio(16/9);

    Qt3DRender::QRenderSurfaceSelector *renderSurfaceSelector = new Qt3DRender::QRenderSurfaceSelector(root);

    Qt3DRender::QViewport *viewport = new Qt3DRender::QViewport(renderSurfaceSelector);

    viewport->setNormalizedRect(QRect(0, 0, 1, 1));

    Qt3DRender::QCameraSelector *cameraSelector = new Qt3DRender::QCameraSelector(viewport);
    cameraSelector->setCamera(camera);

    Qt3DRender::QClearBuffers *clearBuffers = new Qt3DRender::QClearBuffers(cameraSelector);
    clearBuffers->setBuffers(Qt3DRender::QClearBuffers::ColorDepthBuffer);

    // manipulator
    Qt3DExtras::QOrbitCameraController* manipulator = new Qt3DExtras::QOrbitCameraController(root);
    manipulator->setLinearSpeed(50.f);
    manipulator->setLookSpeed(180.f);
    manipulator->setCamera(camera);

    Qt3DCore::QEntity *skybox = addSkybox();
    skybox->setParent(root);

    Qt3DCore::QEntity *plane = new Qt3DCore::QEntity(root);
    Qt3DExtras::QPlaneMesh *planeMesh = new Qt3DExtras::QPlaneMesh(root);
    planeMesh->setWidth(20);
    planeMesh->setHeight(20);
    Qt3DExtras::QPhongMaterial *phongMaterial = new Qt3DExtras::QPhongMaterial(plane);
    phongMaterial->setAmbient(QColor::fromRgba(qRgba(0, 0, 0.7, 1)));
    plane->addComponent(planeMesh);
    plane->addComponent(phongMaterial);

    Qt3DCore::QEntity *scene;
    scene = createScene();
    scene->setParent(root);

    view.setRootEntity(root);
    view.setActiveFrameGraph(renderSurfaceSelector);
    view.show();

    return app.exec();
}