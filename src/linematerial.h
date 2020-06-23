#ifndef LINEMATERIAL_H
#define LINEMATERIAL_H

#include <QObject>
#include <QSize>
#include <Qt3DRender/QMaterial>
#include <Qt3DRender/QTexture>

class LineMaterial : public Qt3DRender::QMaterial
{

    Q_OBJECT
public:
    LineMaterial();
};

#endif
