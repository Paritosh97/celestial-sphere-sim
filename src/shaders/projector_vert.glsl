#version 450 core

in vec3 pos;

layout(location = 0) in vec2 ra_dec;

layout(location = 1) uniform int projection_mode;    // TODO should get projection mode(0-5)

double lambert_projectionK(double x)
{
    return sqrt(2.0 / (1.0 + x));
}

double azimuthal_projectionK(double x)
{
    double crad = acos(x);
    return ((crad != 0) ? crad / sin(crad) : 1);
}

double orthographic_projectionK(double x)
{
    return 1.0;
}

dvec2 equirectangular_projection()
{
    double Y, dX;
    dvec2 screen_pos;

    double zoomFactor = 1;  // TODO Should get zoomfactor
    double height = 1080;   // TODO Should get height
    double width = 1920;    // TODO should get width
    dvec2 focus = (0, 0);   // TODO Should get ra nad dec of the focus

    dX   = ra_dec.x - focus.x;
    Y    = ra_dec.y;
    screen_pos.y = 0.5 * height - zoomFactor * (Y - focus.y);

    dX = reduceAngle(dX, -3.1415, 3.1415);

    screen_pos.x = 0.5 * width - zoomFactor * dX;

    // TODO visiblity

    return screen_pos;
}

double stereographic_projectionK(double x)
{
    return sqrt(2.0 / (1.0 + x));
}

double gnomonic_projectionK(double x)
{
    return sqrt(2.0 / (1.0 + x));
}

double reduceAngle(double x, double minimum, double maximum)
{
    double delta = maximum - minimum;
    return (x - delta * floor((x - minimum) / delta));
}

dvec2 projector()
{
    if(projection_mode == 3)
    {
        return equirectangular_projection();
    }

    double zoomFactor = 1;  // TODO Should get zoomfactor
    double height = 1080;   // TODO Should get height
    double width = 1920;    // TODO should get width
    dvec2 focus = (0, 0);   // TODO Should get ra nad dec of the focus    

    double Y, dX;
    double sindX, cosdX, sinY, cosY;

    dX = ra_dec.x;
    Y = ra_dec.y;

    dX = reduceAngle(dX, -3.1415, 3.1415);

    //Convert dX, Y coords to screen pixel coords
    sindX = sin(dX);
    cosdX = cos(dX);
    sinY  = sin(Y);
    cosY  = cos(Y);

    //c is the cosine of the angular distance from the center
    double c = m_sinY0 * sinY + m_cosY0 * cosY * cosdX;

    // TODO visibility
    
    // TODO switch projection mode
    switch(projection_mode)
    {
        case 0:
            double k = lambert_projectionK(c);
            break;

        case 1:
            double k = azimuthal_projectionK(c);
            break;

        case 2:
            double k = orthographic_projectionK(c);
            break;

        case 4:
            double k = stereographic_projectionK(c);
            break;

        case 5:
            double k = gnomonic_projectionK(c);
            break;
    }    

    double origX = width/2;
    double origY = height/2;
    
    dvec2 screen_pos;

    screen_pos.x = origX - zoomFactor * k * cosY * sindX;
    screen_pos.y = origY - zoomFactor * k * (m_cosY0 * sinY - m_sinY0 * cosY * cosdX);

    return screen_pos;
}

void main() {
    pos = projector();
    gl_Position = vec4(pos.xy, 0, 1);
}
