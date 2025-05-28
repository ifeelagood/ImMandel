cbuffer Params : register(b0)
{
	uint max_it;
};

Texture2D<uint> tex : register(t0);

float4 main(float4 pos : SV_Position) : SV_Target
{
	uint val = tex.Load(int3(pos.xy, 0));
	float gray = val / float(max_it);
	return float4(gray, gray, gray, 1.0);
}