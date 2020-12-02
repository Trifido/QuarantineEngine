#include "ProceduralTexture.h"

void ProceduralTexture::GeneratePermutation()
{
    static int permutation[] = {
            151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,
            8,99,37,240,21,10,23,190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,
            35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,
            134,139,48,27,166,77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,
            55,46,245,40,244,102,143,54, 65,25,63,161,1,216,80,73,209,76,132,187,208, 89,
            18,169,200,196,135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,
            250,124,123,5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,
            189,28,42,223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167,
            43,172,9,129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,
            97,228,251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,
            107,49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
            138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180 };

    p = new int[512];
    for (int x = 0; x < 512; x++) {
        p[x] = permutation[x % 256];
    }
}

double ProceduralTexture::perlin(double x, double y, double z) {
    if (repeat > 0) {									// If we have any repeat on, change the coordinates to their "local" repetitions
        x = (int)x % repeat;
        y = (int)y % repeat;
        z = (int)z % repeat;
    }

    int xi = (int)x & 255;								// Calculate the "unit cube" that the point asked will be located in
    int yi = (int)y & 255;								// The left bound is ( |_x_|,|_y_|,|_z_| ) and the right bound is that
    int zi = (int)z & 255;								// plus 1.  Next we calculate the location (from 0.0 to 1.0) in that cube.
    double xf = x - (int)x;								// We also fade the location to smooth the result.
    double yf = y - (int)y; 
    double zf = z - (int)z;
    double u = fade(xf);
    double v = fade(yf);
    double w = fade(zf);

    int aaa, aba, aab, abb, baa, bba, bab, bbb;
    aaa = p[p[p[xi] + yi] + zi];
    aba = p[p[p[xi] + inc(yi)] + zi];
    aab = p[p[p[xi] + yi] + inc(zi)];
    abb = p[p[p[xi] + inc(yi)] + inc(zi)];
    baa = p[p[p[inc(xi)] + yi] + zi];
    bba = p[p[p[inc(xi)] + inc(yi)] + zi];
    bab = p[p[p[inc(xi)] + yi] + inc(zi)];
    bbb = p[p[p[inc(xi)] + inc(yi)] + inc(zi)];

    double x1, x2, y1, y2;
    x1 = lerp(grad(aaa, xf, yf, zf),				// The gradient function calculates the dot product between a pseudorandom
        grad(baa, xf - 1, yf, zf),				// gradient vector and the vector from the input coordinate to the 8
        u);										// surrounding points in its unit cube.
    x2 = lerp(grad(aba, xf, yf - 1, zf),				// This is all then lerped together as a sort of weighted average based on the faded (u,v,w)
        grad(bba, xf - 1, yf - 1, zf),				// values we made earlier.
        u);
    y1 = lerp(x1, x2, v);

    x1 = lerp(grad(aab, xf, yf, zf - 1),
        grad(bab, xf - 1, yf, zf - 1),
        u);
    x2 = lerp(grad(abb, xf, yf - 1, zf - 1),
        grad(bbb, xf - 1, yf - 1, zf - 1),
        u);
    y2 = lerp(x1, x2, v);

    return (lerp(y1, y2, w) + 1) / 2;						// For convenience we bound it to 0 - 1 (theoretical min/max before is -1 - 1)
}
void ProceduralTexture::ComputePerlin(unsigned int seed)
{
    rawTexture = new unsigned char[tamTexture * tamTexture * nrChannels];
    unsigned int kk = 0;
    // Visit every pixel of the image and assign a color generated with Perlin noise
    for (unsigned int i = 0; i < tamTexture; ++i) {     // y
        for (unsigned int j = 0; j < tamTexture; ++j) {  // x
            double x = (double)j / ((double)tamTexture);
            double y = (double)i / ((double)tamTexture);
            
            // Wood like structure
            double n = OctavePerlin(x, y, 0.0, 8.0, 0.8);//Noise(10 * x, 10 * y, 0.8);
            
            // Map the values to the [0, 255] interval, for simplicity we use
            // tones of grey
            rawTexture[kk] = (unsigned char)floor(255 * n);
            rawTexture[kk+1] = (unsigned char)floor(255 * n);
            rawTexture[kk+2] = (unsigned char)floor(255 * n);
            kk+=3;  
        }
    }
}
void ProceduralTexture::ComputeWorley(unsigned int numBump)
{
    //Worley Noise
    std::vector<glm::vec3> points;
    points.resize(numBump);

    srand(time(NULL));

    for (int i = 0; i < numBump; i++) {
        points[i].x = rand() % tamTexture;
        points[i].y = rand() % tamTexture;
        points[i].z = rand() % tamTexture;
    }

    std::vector<std::vector<float>> distBuffer;
    std::vector<std::vector<float>> worleyNoise;

    distBuffer.resize(tamTexture);
    for (int i = 0; i < tamTexture; i++)
        distBuffer[i].resize(tamTexture);

    worleyNoise.resize(tamTexture);
    for (int i = 0; i < tamTexture; i++)
        worleyNoise[i].resize(tamTexture);

    float maxDist = 0.0, minDist = INFINITY;

    for (unsigned int i = 0; i < tamTexture; i++) {
        for (unsigned int j = 0; j < tamTexture; j++) {
            float value = WorleyNoise(glm::vec3(i, j, 0), points, numBump, &maxDist, &minDist);
            distBuffer[i][j] = value;
        }
    }

    for (unsigned int i = 0; i < tamTexture; i++) {
        for (unsigned int j = 0; j < tamTexture; j++) {
            worleyNoise[i][j] = (distBuffer[i][j] - minDist) / (maxDist - minDist);
        }
    }

    MapGradient(worleyNoise);
}
void ProceduralTexture::MapGradient(std::vector<std::vector<float>> noise)
{
    int width = noise.size();
    int height = noise[0].size();

    rawTexture = new unsigned char[tamTexture * tamTexture * nrChannels];
    unsigned int kk = 0;

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            rawTexture[kk] = (unsigned char)(255 - floor(255 * noise[i][j]));
            rawTexture[kk + 1] = (unsigned char)(255 - floor(255 * noise[i][j]));
            rawTexture[kk + 2] = (unsigned char)(255 - floor(255 * noise[i][j]));

            //rawTexture[kk] = (unsigned char)(floor(255 * noise[i][j]));
            //rawTexture[kk + 1] = (unsigned char)(floor(255 * noise[i][j]));
            //rawTexture[kk + 2] = (unsigned char)(floor(255 * noise[i][j]));

            kk += 3;
        }
    }
}
float ProceduralTexture::WorleyNoise(glm::vec3 input, std::vector<glm::vec3> points, int size, float* maxDist, float* minDist)
{
    float min = INFINITY;

    for (unsigned int i = 0; i < size; ++i) {
        float dist = WrapDist(input, points[i]);

        if (dist < min) min = dist;
    }

    if (min < *minDist) *minDist = min;
    if (min > *maxDist) *maxDist = min;
    return min;
}

glm::vec2 ProceduralTexture::random2(glm::vec2 p) {
    return fract(sin(glm::vec2(glm::dot(p, glm::vec2(127.1, 311.7)), glm::dot(p, glm::vec2(269.5, 183.3)))) * 43758.5453f);
}

float ProceduralTexture::WrapDist(glm::vec3 pixel, glm::vec3 point)
{
    float dx = abs(pixel.x - point.x);
    float dy = abs(pixel.y - point.y);

    if (dx > tamTexture / 2)
        dx = tamTexture - dx;
    if (dy > tamTexture / 2)
        dy = tamTexture - dy;

    return sqrt(dx * dx + dy * dy);
}

double ProceduralTexture::Noise(double x, double y, double z) {
    // Find the unit cube that contains the point
    int X = (int)floor(x) & 255;
    int Y = (int)floor(y) & 255;
    int Z = (int)floor(z) & 255;

    // Find relative x, y,z of point in cube
    x -= floor(x);
    y -= floor(y);
    z -= floor(z);

    // Compute fade curves for each of x, y, z
    double u = fade(x);
    double v = fade(y);
    double w = fade(z);

    // Hash coordinates of the 8 cube corners
    int A = p[X] + Y;
    int AA = p[A] + Z;
    int AB = p[A + 1] + Z;
    int B = p[X + 1] + Y;
    int BA = p[B] + Z;
    int BB = p[B + 1] + Z;

    // Add blended results from 8 corners of cube
    double res = lerp(w, lerp(v, lerp(u, grad(p[AA], x, y, z), grad(p[BA], x - 1, y, z)), lerp(u, grad(p[AB], x, y - 1, z), grad(p[BB], x - 1, y - 1, z))), lerp(v, lerp(u, grad(p[AA + 1], x, y, z - 1), grad(p[BA + 1], x - 1, y, z - 1)), lerp(u, grad(p[AB + 1], x, y - 1, z - 1), grad(p[BB + 1], x - 1, y - 1, z - 1))));
    return (res + 1.0) / 2.0;
}

ProceduralTexture::ProceduralTexture()
{
    
}

ProceduralTexture::ProceduralTexture(NoiseType type, unsigned int tamTexture, unsigned int nrChannels, unsigned int numRandomPoints)
{
    typeNoise = type;
    this->tamTexture = tamTexture;
    this->nrChannels = nrChannels;
    this->seed = numRandomPoints;
    this->numBump = numRandomPoints;
    this->repeat = 0;

    GLenum format = GetFormat();

    switch (type)
    {
    case PERLIN:
        GeneratePermutation();
        ComputePerlin(numRandomPoints);

        glGenTextures(1, &ID);
        glBindTexture(GL_TEXTURE_2D, ID);

        glTexImage2D(GL_TEXTURE_2D, 0, format, tamTexture, tamTexture, 0, format, GL_UNSIGNED_BYTE, rawTexture);
        glGenerateMipmap(GL_TEXTURE_2D);

        break;
    case WORLEY:
        ComputeWorley(numRandomPoints);
        glGenTextures(1, &ID);
        glBindTexture(GL_TEXTURE_2D, ID);

        glTexImage2D(GL_TEXTURE_2D, 0, format, tamTexture, tamTexture, 0, format, GL_UNSIGNED_BYTE, rawTexture);
        glGenerateMipmap(GL_TEXTURE_2D);
        break;
    default:
        GeneratePermutation();
        ComputePerlin(numRandomPoints);

        glGenTextures(1, &ID);
        glBindTexture(GL_TEXTURE_2D, ID);

        glTexImage2D(GL_TEXTURE_2D, 0, format, tamTexture, tamTexture, 0, format, GL_UNSIGNED_BYTE, rawTexture);
        glGenerateMipmap(GL_TEXTURE_2D);
        break;
    }
}

double ProceduralTexture::fade(double t) {
    return t * t * t * (t * (t * 6 - 15) + 10);
}

double ProceduralTexture::lerp(double t, double a, double b) {
    //return a + t * (b - a);
    return t + b * (a - t);
}

double ProceduralTexture::grad(int hash, double x, double y, double z) {
    int h = hash & 15;
    // Convert lower 4 bits of hash into 12 gradient directions
    double u = h < 8 ? x : y,
        v = h < 4 ? y : h == 12 || h == 14 ? x : z;
    return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

int ProceduralTexture::inc(int num) {
    num++;
    if (repeat > 0) num %= repeat;

    return num;
}

double ProceduralTexture::OctavePerlin(double x, double y, double z, int octaves, double persistence) {
    double total = 0;
    double frequency = 1;
    double amplitude = 1;
    double maxValue = 0;			// Used for normalizing result to 0.0 - 1.0
    for (int i = 0; i < octaves; i++) {
        total += perlin(x * frequency, y * frequency, z * frequency) * amplitude;

        maxValue += amplitude;

        amplitude *= persistence;
        frequency *= 2;
    }

    return total / maxValue;
}

GLenum ProceduralTexture::GetFormat()
{
    if (nrChannels == 1)
        return GL_RED;
    else if (nrChannels == 3)
        return GL_RGB;
    else 
        return GL_RGBA;
}

