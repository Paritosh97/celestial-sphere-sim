#version 450 core

in vec3 vertexPosition;
in vec3 vertexNormal;
in vec3 pos;

out vec3 worldPosition;
out vec3 worldNormal;

uniform mat4 modelView;
uniform mat3 modelViewNormal;
uniform mat4 modelViewProjection;

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

    highp float zoomFactor = 1;  // TODO Should get zoomfactor
    highp float height = 10;   // TODO Should get height
    highp float width = 15;    // TODO should get width
    highp vec2 focus = vec2(0, 0);   // TODO Should get ra nad dec of the focus

    dX   = pos.x - focus.x;
    Y    = pos.y;
    screen_pos.y = 0.5 * height - zoomFactor * (Y - focus.y);

    dX = reduceAngle(dX, -3.1415, 3.1415);

    screen_pos.x = 0.5 * width - zoomFactor * dX;

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
    int projection_mode = 0;    // TODO Should get projection mode
    highp float zoomFactor = 1;  // TODO Should get zoomfactor
    highp float height = 10;   // TODO Should get height
    highp float width = 5;    // TODO should get width
    highp vec2 focus = vec2(0, 0);   // TODO Should get ra nad dec of the focus 
    float m_sinY0 = 0;  // TODO sin for focus point
    float m_cosY0 = 1;  // TODO cos for focus point

    if(projection_mode == 3)
    {
        return equirectangular_projection();
    }

    highp float Y, dX;
    highp float sindX, cosdX, sinY, cosY;

    dX = highp float(pos.x);
    Y = highp float(pos.y);

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
    
    highp vec3 screen_pos;

    screen_pos.x = origX - zoomFactor * k * cosY * sindX;
    screen_pos.y = origY - zoomFactor * k * (m_cosY0 * sinY - m_sinY0 * cosY * cosdX);
    screen_pos.z = 0;

    return screen_pos;
}

void main()
{
    highp vec3 projected_pos = projector();
    
    vec4 offsetPos = vec4(vertexPosition, 1.0) + vec4(projected_pos, 0.0);

    worldNormal = normalize(vertexNormal);
    worldPosition = vec3(offsetPos);

    gl_Position = modelViewProjection * offsetPos;
}