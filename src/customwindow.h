#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>

#include <Qt3DRender/QViewport>

#include <QObject>
#include <Qt3DExtras/Qt3DWindow>

#include "linegeometry.h"
#include "linematerial.h"

#include "skypoint.h"
#include "linelist.h"
#include "typedef.h"

class SkyPoint;

class CustomWindow : public Qt3DExtras::Qt3DWindow
{
    public:
    explicit CustomWindow();

    protected:
    void resizeEvent(QResizeEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

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
