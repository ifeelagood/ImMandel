float4 main(uint vertexID : SV_VertexID) : SV_Position
{
    float2 pos = float2((vertexID << 1) & 2, vertexID & 2);
    float2 screen = pos * 2.0f - 1.0f;
    return float4(screen.x, -screen.y, 0.0f, 1.0f);
}