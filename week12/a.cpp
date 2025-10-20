#include <bits/stdc++.h>
#include <omp.h>
using namespace std;

struct Pixel {
    int r, g, b;
};

struct Image {
    int N, M;
    vector<Pixel> data;
};

Image read_image(istream &in) {
    Image img;
    img.data.resize(img.N * img.M);
    for (int i = 0; i < img.N * img.M; ++i) {
        in >> img.data[i].r >> img.data[i].g >> img.data[i].b;
    }
    return img;
}

void print(const Image &img, ostream &out) {
    out << img.N << " " << img.M << "\n";
    for (int i = 0; i < img.N * img.M; ++i) {
        out << img.data[i].r << " " << img.data[i].g << " " << img.data[i].b << "\n";
    }
}

Image filterBK(const Image &img) {
    Image out = img;
    for (int i = 0; i < img.N * img.M; ++i) {
        int y = (img.data[i].r + img.data[i].g + img.data[i].b) / 3;
        out.data[i] = {y, y, y};
    }
    return out;
}

int main() {

    int n;
    vector<Image> imgs(n);
    for (int i = 0; i < n; ++i)
        imgs[i] = read_image(cin);

    vector<Image> results(n);

    #pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < n; ++i)
        results[i] = filterBK(imgs[i]);

    for (int i = 0; i < n; ++i)
        print(results[i], cout);

    return 0;
}
