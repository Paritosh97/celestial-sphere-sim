#version 150

in highp vec2 vertexPosition;

uniform mat4 modelMatrix;
uniform mat4 mvp;

uniform int projection_mode;    // TODO should get projection mode(0-5)

out vec3 worldPosition;

highp float reduceAngle(highp float x, highp float minimum, highp float maximum)
{
    highp float delta = maximum - minimum;
    return (x - delta * floor((x - minimum) / delta));
}

highp float lambert_projectionK(highp float x)
{
    return sqrt(2.0 / (1.0 + x));
}

highp float azimuthal_projectionK(highp float x)
{
    highp float crad = acos(x);
    return ((crad != 0) ? crad / sin(crad) : 1);
}

highp float orthographic_projectionK(highp float x)
{
    return 1.0;
}

highp vec2 equirectangular_projection()
{
    highp float Y, dX;
    highp vec2 screen_pos;

    highp float zoomFactor = 1;  // TODO Should get zoomfactor
    highp float height = 1080;   // TODO Should get height
    highp float width = 1920;    // TODO should get width
    highp vec2 focus = vec2(0, 0);   // TODO Should get ra nad dec of the focus

    dX   = vertexPosition.x - focus.x;
    Y    = vertexPosition.y;
    screen_pos.y = 0.5 * height - zoomFactor * (Y - focus.y);

    dX = reduceAngle(dX, -3.1415, 3.1415);

    screen_pos.x = 0.5 * width - zoomFactor * dX;

    // TODO visiblity

    return screen_pos;
}

float stereographic_projectionK(float x)
{
    return sqrt(2.0 / (1.0 + x));
}

float gnomonic_projectionK(float x)
{
    return sqrt(2.0 / (1.0 + x));
}

highp vec2 projector()
{
    if(projection_mode == 3)
    {
        return equirectangular_projection();
    }

    highp float zoomFactor = 1;  // TODO Should get zoomfactor
    highp float height = 1080;   // TODO Should get height
    highp float width = 1920;    // TODO should get width
    highp vec2 focus = vec2(0, 0);   // TODO Should get ra nad dec of the focus 
    float m_sinY0 = 0;  // TODO sin for focus point
    float m_cosY0 = 1;  // TODO cos for focus point

    highp float Y, dX;
    highp float sindX, cosdX, sinY, cosY;

    dX = highp float(vertexPosition.x);
    Y = highp float(vertexPosition.y);

    dX = reduceAngle(dX, -3.1415, 3.1415);

    //Convert dX, Y coords to screen pixel coords
    sindX = sin(dX);
    cosdX = cos(dX);
    sinY  = sin(Y);
    cosY  = cos(Y);

    //c is the cosine of the angular distance from the center
    highp float c = m_sinY0 * sinY + m_cosY0 * cosY * cosdX;

    // TODO visibility
    
    // TODO switch projection mode
    highp float k = 0;
    switch(projection_mode)
    {
        case 0:
            k = lambert_projectionK(c);
            break;

        case 1:
            k = azimuthal_projectionK(c);
            break;

        case 2:
            k = orthographic_projectionK(c);
            break;

        case 4:
            k = stereographic_projectionK(c);
            break;

        case 5:
            k = gnomonic_projectionK(c);
            break;
    }    

    highp float origX = width/2;
    highp float origY = height/2;
    
    highp vec2 screen_pos;

    screen_pos.x = origX - zoomFactor * k * cosY * sindX;
    screen_pos.y = origY - zoomFactor * k * (m_cosY0 * sinY - m_sinY0 * cosY * cosdX);

    return screen_pos;
}

void main() {
    highp vec2 pos = projector();
    worldPosition = vec3(modelMatrix * vec4(pos, 0, 1));
    gl_Position = mvp * vec4(pos, 0, 1);
}
