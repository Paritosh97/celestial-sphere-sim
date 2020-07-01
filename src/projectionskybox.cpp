#include <Qt3DRender/qfilterkey.h>
#include <Qt3DRender/qeffect.h>
#include <Qt3DRender/qtexture.h>
#include <Qt3DRender/qmaterial.h>
#include <Qt3DRender/qcullface.h>
#include <Qt3DRender/qdepthtest.h>
#include <Qt3DRender/qparameter.h>
#include <Qt3DRender/qtechnique.h>
#include <Qt3DRender/qrenderpass.h>
#include <Qt3DRender/qgraphicsapifilter.h>
#include <Qt3DRender/qseamlesscubemap.h>
#include <Qt3DRender/qshaderprogram.h>

#include <Qt3DExtras/qcuboidmesh.h>

#include <projectionskybox.h>

using namespace Qt3DCore;
using namespace Qt3DRender;

ProjectionSkybox::ProjectionSkybox(QString m_baseName, QNode *parent)
    : QEntity()
    , m_effect(new QEffect())
    , m_material(new QMaterial())
    , m_skyboxTexture(new QTextureCubeMap())
    , m_loadedTexture(new QTextureLoader())
    , m_gl3Shader(new QShaderProgram())
    , m_gl3Technique(new QTechnique())
    , m_filterKey(new QFilterKey)
    , m_gl3RenderPass(new QRenderPass())
    , m_mesh(new Qt3DExtras::QCuboidMesh())
    , m_gammaStrengthParameter(new QParameter(QStringLiteral("gammaStrength"), 0.0f))
    , m_textureParameter(new QParameter(QStringLiteral("skyboxTexture"), m_skyboxTexture))
    , m_posXImage(new QTextureImage())
    , m_posYImage(new QTextureImage())
    , m_posZImage(new QTextureImage())
    , m_negXImage(new QTextureImage())
    , m_negYImage(new QTextureImage())
    , m_negZImage(new QTextureImage())
    , m_extension(QStringLiteral(".png"))
    , m_hasPendingReloadTextureCall(false)
{
    m_loadedTexture->setGenerateMipMaps(false);

    m_gl3Shader->setVertexShaderCode(QShaderProgram::loadSource((QUrl::fromLocalFile( "/home/paritosh/Desktop/gsoc/celestial-sphere-sim/src/shaders/skybox.vert") )));
    m_gl3Shader->setFragmentShaderCode(QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/shaders/gl3/skybox.frag"))));

    m_gl3Technique->graphicsApiFilter()->setApi(QGraphicsApiFilter::OpenGL);
    m_gl3Technique->graphicsApiFilter()->setMajorVersion(3);
    m_gl3Technique->graphicsApiFilter()->setMinorVersion(3);
    m_gl3Technique->graphicsApiFilter()->setProfile(QGraphicsApiFilter::CoreProfile);

    m_filterKey->setParent(m_effect);
    m_filterKey->setName(QStringLiteral("renderingStyle"));
    m_filterKey->setValue(QStringLiteral("forward"));

    m_gl3Technique->addFilterKey(m_filterKey);

    m_gl3RenderPass->setShaderProgram(m_gl3Shader);

    QCullFace *cullFront = new QCullFace();
    cullFront->setMode(QCullFace::Front);
    QDepthTest *depthTest = new QDepthTest();
    depthTest->setDepthFunction(QDepthTest::LessOrEqual);
    QSeamlessCubemap *seamlessCubemap = new QSeamlessCubemap();

    m_gl3RenderPass->addRenderState(cullFront);
    m_gl3RenderPass->addRenderState(depthTest);
    m_gl3RenderPass->addRenderState(seamlessCubemap);

    m_gl3Technique->addRenderPass(m_gl3RenderPass);

    m_effect->addTechnique(m_gl3Technique);

    m_material->setEffect(m_effect);
    m_material->addParameter(m_gammaStrengthParameter);
    m_material->addParameter(m_textureParameter);

    m_mesh->setXYMeshResolution(QSize(2, 2));
    m_mesh->setXZMeshResolution(QSize(2, 2));
    m_mesh->setYZMeshResolution(QSize(2, 2));

    m_posXImage->setFace(QTextureCubeMap::CubeMapPositiveX);
    m_posXImage->setMirrored(false);
    m_posYImage->setFace(QTextureCubeMap::CubeMapPositiveY);
    m_posYImage->setMirrored(false);
    m_posZImage->setFace(QTextureCubeMap::CubeMapPositiveZ);
    m_posZImage->setMirrored(false);
    m_negXImage->setFace(QTextureCubeMap::CubeMapNegativeX);
    m_negXImage->setMirrored(false);
    m_negYImage->setFace(QTextureCubeMap::CubeMapNegativeY);
    m_negYImage->setMirrored(false);
    m_negZImage->setFace(QTextureCubeMap::CubeMapNegativeZ);
    m_negZImage->setMirrored(false);

    m_skyboxTexture->setMagnificationFilter(QTextureCubeMap::Linear);
    m_skyboxTexture->setMinificationFilter(QTextureCubeMap::Linear);
    m_skyboxTexture->setGenerateMipMaps(false);
    m_skyboxTexture->setWrapMode(QTextureWrapMode(QTextureWrapMode::ClampToEdge));

    m_posXImage->setSource(QUrl(m_baseName + QStringLiteral("_posx") + m_extension));
    m_posYImage->setSource(QUrl(m_baseName + QStringLiteral("_posy") + m_extension));
    m_posZImage->setSource(QUrl(m_baseName + QStringLiteral("_posz") + m_extension));
    m_negXImage->setSource(QUrl(m_baseName + QStringLiteral("_negx") + m_extension));
    m_negYImage->setSource(QUrl(m_baseName + QStringLiteral("_negy") + m_extension));
    m_negZImage->setSource(QUrl(m_baseName + QStringLiteral("_negz") + m_extension));
    m_textureParameter->setValue(QVariant::fromValue(m_skyboxTexture));

    m_skyboxTexture->addTextureImage(m_posXImage);
    m_skyboxTexture->addTextureImage(m_posYImage);
    m_skyboxTexture->addTextureImage(m_posZImage);
    m_skyboxTexture->addTextureImage(m_negXImage);
    m_skyboxTexture->addTextureImage(m_negYImage);
    m_skyboxTexture->addTextureImage(m_negZImage);


    this->addComponent(m_mesh);
    this->addComponent(m_material);
}

