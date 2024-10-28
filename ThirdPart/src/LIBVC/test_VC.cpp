#include <Vc/Vc>
#include <iomanip>

using Vc::float_v;

int main()
{
    // allocate memory for our initial x and y coordinates. Note that you can also put it into a
    // normal float C-array but that you then must ensure alignment to Vc::VectorAlignment!
    Vc::Memory<float_v, 1000> x_mem;
    Vc::Memory<float_v, 1000> y_mem;
    Vc::Memory<float_v, 1000> r_mem;
    Vc::Memory<float_v, 1000> phi_mem;

    // fill the memory with values from -1.f to 1.f
    for (size_t i = 0; i < x_mem.vectorsCount(); ++i) {
        x_mem.vector(i) = float_v::Random() * 2.f - 1.f;
        y_mem.vector(i) = float_v::Random() * 2.f - 1.f;
    }

    // calculate the polar coordinates for all coordinates and overwrite the euclidian coordinates
    // with the result
    for (size_t i = 0; i < x_mem.vectorsCount(); ++i) {
        const float_v x = x_mem.vector(i);
        const float_v y = y_mem.vector(i);
        r_mem.vector(i) = Vc::sqrt(x * x + y * y);
        float_v phi = Vc::atan2(y, x) * 57.295780181884765625f; // 180/pi
        phi(phi < 0.f) += 360.f;
        phi_mem.vector(i) = phi;
    }

    // print the results
    for (size_t i = 0; i < x_mem.entriesCount(); ++i) {
        std::cout << std::setw(3) << i << ": ";
        std::cout << std::setw(10) << x_mem[i] << ", " << std::setw(10) << y_mem[i] << " -> ";
        std::cout << std::setw(10) << r_mem[i] << ", " << std::setw(10) << phi_mem[i] << '\n';
    }
    return 0;
}

