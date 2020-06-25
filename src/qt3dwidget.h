#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>

#include <Qt3DRender/QViewport>

#include <Qt3DExtras/Qt3DWindow>

#include <QObject>
#include <QWidget>

#include "linegeometry.h"
#include "linematerial.h"

#include "skypoint.h"
#include "linelist.h"
#include "typedef.h"

class SkyPoint;

class Qt3DWidget : public QWidget
{
    Q_OBJECT
    QWidget *container;

    public:
    explicit Qt3DWidget(QWidget *parent = 0);

    private:
    Qt3DCore::QEntity* addSkybox();
    Qt3DCore::QEntity* createScene();
    Qt3DCore::QEntity* addLines();
    Qt3DCore::QEntity* addSkyPolyline(LineList *lineList);
    Qt3DCore::QEntity* addLine(SkyPoint *pLast, SkyPoint *pThis);
    std::shared_ptr<LineListList> fillLineListList();
    void fillLineList(std::shared_ptr<LineList> lineList, int j);

    Qt3DRender::QViewport *viewport;
};
