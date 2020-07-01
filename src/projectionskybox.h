#include <Qt3DCore/QEntity>

#include <QVector3D>

namespace Qt3DRender {

class QFilterKey;
class QTextureCubeMap;
class QTextureLoader;
class QShaderProgram;
class QSkyboxEntity;
class QTextureImage;
class QRenderPass;
class QTechnique;
class QParameter;
class QMaterial;
class QEffect;

}

#include <Qt3DExtras/QCuboidMesh>

class ProjectionSkybox : public Qt3DCore::QEntity
{
public:
    explicit ProjectionSkybox(QString m_baseName, Qt3DCore::QNode *parent = nullptr);

private:
    Qt3DRender::QEffect *m_effect;
    Qt3DRender::QMaterial *m_material;
    Qt3DRender::QTextureCubeMap *m_skyboxTexture;
    Qt3DRender::QTextureLoader *m_loadedTexture;
    Qt3DRender::QShaderProgram *m_gl3Shader;
    Qt3DRender::QTechnique *m_gl3Technique;
    Qt3DRender::QFilterKey *m_filterKey;
    Qt3DRender::QRenderPass *m_gl3RenderPass;
    Qt3DExtras::QCuboidMesh *m_mesh;
    Qt3DRender::QParameter *m_gammaStrengthParameter;
    Qt3DRender::QParameter *m_textureParameter;
    Qt3DRender::QTextureImage *m_posXImage;
    Qt3DRender:: QTextureImage *m_posYImage;
    Qt3DRender::QTextureImage *m_posZImage;
    Qt3DRender::QTextureImage *m_negXImage;
    Qt3DRender::QTextureImage *m_negYImage;
    Qt3DRender::QTextureImage *m_negZImage;
    QString m_extension;
    QVector3D m_position;
    bool m_hasPendingReloadTextureCall;

};