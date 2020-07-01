#version 330

in vec3 vertexPosition;
out vec3 texCoord0;

uniform mat4 mvp;
uniform mat4 inverseProjectionMatrix;
uniform mat4 inverseModelView;

uniform float zoomFactor = 1;
uniform vec2 focus = vec2(0, 0);    // TODO
uniform int projection_mode = 0;

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

highp vec3 equirectangular_projection()
{
    highp float Y, dX;
    highp vec3 screen_pos;

    dX   = vertexPosition.x - focus.x;
    Y    = vertexPosition.y;

    // Centre of screen
    highp float origX =  0;
    highp float origY =  0;
    
    screen_pos.y = 0.5 * origY - zoomFactor * (Y - focus.y);

    dX = reduceAngle(dX, -3.1415, 3.1415);

    screen_pos.x = 0.5 * origX - zoomFactor * dX;

    screen_pos.z = 0;

    // TODO visiblity

    return screen_pos;
}

highp float stereographic_projectionK(float x)
{
    return sqrt(2.0 / (1.0 + x));
}

highp float gnomonic_projectionK(float x)
{
    return sqrt(2.0 / (1.0 + x));
}

highp vec3 projector()
{
    float m_sinY0 = 0;  // TODO sin for focus point
    float m_cosY0 = 1;  // TODO cos for focus point

    if(projection_mode == 3)
    {
        return equirectangular_projection();
    }

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
    
    highp float k = 1;
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

    // Centre of screen
    highp float origX =  0;
    highp float origY =  0;
    
    highp vec3 screen_pos;

    screen_pos.x = origX - zoomFactor * k * cosY * sindX;
    screen_pos.y = origY - zoomFactor * k * (m_cosY0 * sinY - m_sinY0 * cosY * cosdX);
    screen_pos.z = 0;

    screen_pos.x = clamp(screen_pos.x, -1, 1);
    screen_pos.y = clamp(screen_pos.y, -1, 1);

    return screen_pos;
}

void main()
{
    highp vec3 projected_pos = projector();    

    texCoord0 = projected_pos.xyz;
    gl_Position = vec4(mvp * vec4(projected_pos, 1.0)).xyww;
}