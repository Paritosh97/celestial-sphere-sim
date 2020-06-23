#include <Qt3DRender/QShaderProgram>
#include <Qt3DRender/QEffect>
#include <Qt3DRender/QTechnique>
#include <Qt3DRender/QRenderPass>
#include <Qt3DRender/QGraphicsApiFilter>
#include <Qt3DRender/QParameter>
#include <QUrl>
#include <QSizeF>
#include <QVariant>

#include "linematerial.h"

LineMaterial::LineMaterial()
{
    // Shader program
    Qt3DRender::QShaderProgram *shaderProgram = new Qt3DRender::QShaderProgram( this );
    shaderProgram->setVertexShaderCode( Qt3DRender::QShaderProgram::loadSource(QUrl::fromLocalFile( "/home/paritosh/Desktop/gsoc/celestial-sphere-sim/src/shaders/lines.vert") ) );
    shaderProgram->setFragmentShaderCode( Qt3DRender::QShaderProgram::loadSource(QUrl::fromLocalFile( "/home/paritosh/Desktop/gsoc/celestial-sphere-sim/src/shaders/lines.frag")) );
    
    // Render Pass
    Qt3DRender::QRenderPass *renderPass = new Qt3DRender::QRenderPass( this );
    renderPass->setShaderProgram(shaderProgram);

    // without this filter the default forward renderer would not render this
    Qt3DRender::QFilterKey *filterKey = new Qt3DRender::QFilterKey;
    filterKey->setName( QStringLiteral( "renderingStyle" ) );
    filterKey->setValue( "forward" );

    // Technique
    Qt3DRender::QTechnique *technique = new Qt3DRender::QTechnique;
    technique->addRenderPass(renderPass);
    technique->addFilterKey(filterKey);
    technique->graphicsApiFilter()->setApi( Qt3DRender::QGraphicsApiFilter::OpenGL );
    technique->graphicsApiFilter()->setProfile( Qt3DRender::QGraphicsApiFilter::CoreProfile );
    technique->graphicsApiFilter()->setMajorVersion( 3 );
    technique->graphicsApiFilter()->setMinorVersion( 1 );

    // Effect
    Qt3DRender::QEffect *effect = new Qt3DRender::QEffect( this );
    effect->addTechnique(technique);

    setEffect( effect );
}