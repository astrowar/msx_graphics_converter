namespace color
{
    // Convert sRGB color in [0..255]^3 to CIE-XYZ.
    void srgb_to_xyz(const unsigned char* srgb, double* xyz);
    void xyz_to_lab(const double* xyz, double* lab);
    double diff_de00(const double* lab1, const double* lab2);
}
