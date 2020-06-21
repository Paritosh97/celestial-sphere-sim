#include <QGuiApplication>

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DCore/QAspectEngine>

#include <Qt3DRender/qrenderaspect.h>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QMaterial>
#include <Qt3DRender/QAttribute>
#include <Qt3DRender/QShaderProgram>
#include <Qt3DRender/QRenderPass>
#include <Qt3DRender/QTechnique>
#include <Qt3DRender/QGraphicsApiFilter>
#include <Qt3DRender/QEffect>
#include <Qt3DRender/QBuffer>

#include <Qt3DExtras/Qt3DWindow>
#include <Qt3DExtras/QTorusMesh>
#include <Qt3DExtras/QOrbitCameraController>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QSkyboxEntity>

#include <QUrl>

#include <QColor>

#include "skypoint.h"
#include "linelist.h"

class SkyPoint;

#include "typedef.h"

Qt3DCore::QEntity* addSkybox()
{
    Qt3DExtras::QSkyboxEntity* skybox = new Qt3DExtras::QSkyboxEntity;
    QString path = QUrl::fromLocalFile("/home/paritosh/Desktop/gsoc/celestial-sphere-sim/skybox/park").toString();
    skybox->setBaseName(path);
    skybox->setExtension(".png");

    return skybox;
}

void fillLineList(std::shared_ptr<LineList> lineList, int j)
{
    for(int i = 1; i < 10; i++)
    {
        std::shared_ptr<SkyPoint>  point(new SkyPoint(i*j*10, i*j*20));
        lineList->append(point);
    }    
}

std::shared_ptr<LineListList> fillLineListList()
{
    std::shared_ptr<LineListList> lineListList(new LineListList);

    for(int i = 1; i < 10; i++)
    {
        std::shared_ptr<LineList> lineList(new LineList);
        fillLineList(lineList, i);
        lineListList->append(lineList);
    }

    return lineListList;
}

Qt3DCore::QEntity* addLine(SkyPoint *pLast, SkyPoint *pThis)
{   
    Qt3DCore::QEntity* lineRoot = new Qt3DCore::QEntity();

    auto *geometry = new Qt3DRender::QGeometry(lineRoot);

    // sending ra and dec
    QByteArray bufferBytes;
    bufferBytes.resize(2 * 2 * sizeof(double));
    double *ra_dec = reinterpret_cast<double*>(bufferBytes.data());
    *ra_dec++ = pLast->ra().radians();
    *ra_dec++ = pLast->dec().radians();
    *ra_dec++ = pThis->ra().radians();
    *ra_dec++ = pThis->dec().radians();

    auto *buf = new Qt3DRender::QBuffer(geometry);
    buf->setData(bufferBytes);

    auto *ra_decAttribute = new Qt3DRender::QAttribute(geometry);
    ra_decAttribute->setName(Qt3DRender::QAttribute::defaultPositionAttributeName());
    ra_decAttribute->setVertexBaseType(Qt3DRender::QAttribute::Float);
    ra_decAttribute->setVertexSize(2);
    ra_decAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    ra_decAttribute->setBuffer(buf);
    ra_decAttribute->setByteStride(2 * sizeof(float));
    ra_decAttribute->setCount(2);
    geometry->addAttribute(ra_decAttribute); // We add the vertices in the geometry

    // connectivity between vertices
    QByteArray indexBytes;
    indexBytes.resize(2 * sizeof(unsigned int)); // start to end
    unsigned int *indices = reinterpret_cast<unsigned int*>(indexBytes.data());
    *indices++ = 0;
    *indices++ = 1;

    auto *indexBuffer = new Qt3DRender::QBuffer(geometry); 
    indexBuffer->setData(indexBytes);

    auto *indexAttribute = new Qt3DRender::QAttribute(geometry);
    indexAttribute->setVertexBaseType(Qt3DRender::QAttribute::UnsignedInt);
    indexAttribute->setAttributeType(Qt3DRender::QAttribute::IndexAttribute);
    indexAttribute->setBuffer(indexBuffer);
    indexAttribute->setCount(2);
    geometry->addAttribute(indexAttribute); // We add the indices linking the points in the geometry

    // mesh    
    auto *line = new Qt3DRender::QGeometryRenderer(lineRoot);
    line->setGeometry(geometry);
    line->setPrimitiveType(Qt3DRender::QGeometryRenderer::Lines);

    Qt3DRender::QMaterial *material = new Qt3DRender::QMaterial();

    Qt3DRender::QShaderProgram *glShader = new Qt3DRender::QShaderProgram();
    QUrl shaderURL = QUrl::fromLocalFile("/home/paritosh/Desktop/gsoc/celestial-sphere-sim/src/shaders/projector.vert");
    glShader->setVertexShaderCode(Qt3DRender::QShaderProgram::loadSource(shaderURL));
    shaderURL = QUrl::fromLocalFile("/home/paritosh/Desktop/gsoc/celestial-sphere-sim/src/shaders/projector.frag");
    glShader->setFragmentShaderCode(Qt3DRender::QShaderProgram::loadSource(shaderURL));
    
    Qt3DRender::QRenderPass *pass = new Qt3DRender::QRenderPass();
    pass->setShaderProgram(glShader);

    Qt3DRender::QTechnique *technique = new Qt3DRender::QTechnique();
    technique->graphicsApiFilter()->setApi(Qt3DRender::QGraphicsApiFilter::OpenGL);
    technique->graphicsApiFilter()->setMajorVersion(3);
    technique->graphicsApiFilter()->setMinorVersion(1);    
    technique->graphicsApiFilter()->setProfile(Qt3DRender::QGraphicsApiFilter::CoreProfile);
    technique->addRenderPass(pass);

    Qt3DRender::QEffect *effect = new Qt3DRender::QEffect(lineRoot);
    effect->addTechnique(technique);

    material->setEffect(effect);

    //Qt3DExtras::QPhongMaterial *material = new Qt3DExtras::QPhongMaterial(lineRoot);

    auto *lineEntity = new Qt3DCore::QEntity(lineRoot);
    lineEntity->addComponent(line);
    lineEntity->addComponent(material);

    return lineRoot;
}

Qt3DCore::QEntity* addSkyPolyline(LineList *lineList)
{
    Qt3DCore::QEntity* skyPolyLine = new Qt3DCore::QEntity();

    SkyList *points = lineList->points();

    for (int j = 1; j < points->size(); j++)
    {
        SkyPoint *pLast = points->at(j++).get();
        SkyPoint *pThis = points->at(j).get();

        //qDebug()<<"Right Ascension : "<<(pThis->ra().degree());
        //qDebug()<<"Decination : "<<(pThis->dec().degree());

        Qt3DCore::QEntity* line = addLine(pLast, pThis);
        line->setParent(skyPolyLine);
    }

    return skyPolyLine;
}

Qt3DCore::QEntity* addLines()
{
    // Add lines on the celestial sphere
    Qt3DCore::QEntity* lines = new Qt3DCore::QEntity();

    std::shared_ptr<LineListList> lineListList = fillLineListList();

    for (int i = 0; i < lineListList->size(); i++)
    {
        std::shared_ptr<LineList> lineList = lineListList->at(i);

        Qt3DCore::QEntity* line = addSkyPolyline(lineList.get());
        line->setParent(lines);
    }
    
    return lines;
}

Qt3DCore::QEntity* createTestScene()
{
    Qt3DCore::QEntity *root = new Qt3DCore::QEntity;

    // add skybox
    Qt3DCore::QEntity *skybox = addSkybox();
    skybox->setParent(root);

    // add polyline
    Qt3DCore::QEntity *lines = addLines();
    lines->setParent(root);

    return root;
}

int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);
    Qt3DExtras::Qt3DWindow view;
    Qt3DCore::QEntity* scene = createTestScene();

    // camera
    Qt3DRender::QCamera *camera = view.camera();
    camera->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.1f, 1000.0f);
    camera->setPosition(QVector3D(0, 0, 40.0f));
    camera->setViewCenter(QVector3D(0, 0, 0));

    // manipulator
    Qt3DExtras::QOrbitCameraController* manipulator = new Qt3DExtras::QOrbitCameraController(scene);
    manipulator->setLinearSpeed(50.f);
    manipulator->setLookSpeed(180.f);
    manipulator->setCamera(camera);
    
    view.setRootEntity(scene);
    view.show();

    return app.exec();
}