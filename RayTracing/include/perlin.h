#pragma once
#include "rtweekend.h"
// more idea refer to GPU gems1
// here is the traditional one
class perlin {
public:
    perlin() {
        ranvec = new vec3[point_count];
        for (int i = 0; i < point_count; ++i) {
            ranvec[i] = unit_vector(vec3::random(-1, 1));
        }
        //这里只用一维随机数生成三维体纹理
        //利用xyz随机数得到在固定随机数轴上的坐标
        //之后结合三个坐标异或运算得到坐标

        //生成xyz交换坐标
        perm_x = perlin_generate_perm();
        perm_y = perlin_generate_perm();
        perm_z = perlin_generate_perm();
    }

    ~perlin() {
        delete[] ranvec;
        delete[] perm_x;
        delete[] perm_y;
        delete[] perm_z;
    }
    //return [-1 1]
    double noise(const point3& p) const {
        
        /*auto i = static_cast<int>(4 * p.x()) & 255;
        auto j = static_cast<int>(4 * p.y()) & 255;
        auto k = static_cast<int>(4 * p.z()) & 255;

        return ranfloat[perm_x[i] ^ perm_y[j] ^ perm_z[k]];*/


        auto u = p.x() - floor(p.x());
        auto v = p.y() - floor(p.y());
        auto w = p.z() - floor(p.z());

        vec3 c[2][2][2];

        auto i = static_cast<int>(floor(p.x()));
        auto j = static_cast<int>(floor(p.y()));
        auto k = static_cast<int>(floor(p.z()));
        //拿到相邻的八个点
        for (int di = 0; di < 2; di++)
            for (int dj = 0; dj < 2; dj++)
                for (int dk = 0; dk < 2; dk++)
                    c[di][dj][dk] = ranvec[
                        perm_x[(i + di) & 255] ^
                            perm_y[(j + dj) & 255] ^
                            perm_z[(k + dk) & 255]
                    ];

        return perlin_interp(c, u, v, w);
    }
    //return [0 1+]
    double turb(const point3& p, int depth = 7) const {
        auto accum = 0.0;
        auto temp_p = p;
        auto weight = 1.0;

        for (int i = 0; i < depth; i++) {
            accum += weight * noise(temp_p);
            weight *= 0.5; // half weight
            temp_p *= 2;// double frequence
        }

        return fabs(accum);
    }
private:
    static const int point_count = 256;
    vec3* ranvec;
    int* perm_x;
    int* perm_y;
    int* perm_z;

    static int* perlin_generate_perm() {
        auto p = new int[point_count];

        //先创建各个位置对应下标
        for (int i = 0; i < perlin::point_count; i++)
            p[i] = i;
        //再随机数生成交换坐标
        permute(p, point_count);

        return p;
    }

    static void permute(int* p, int n) {
        //把目标位置与待放位置（末尾i）交换
        for (int i = n - 1; i > 0; i--) {
            //random_int return [0,i]
            int target = random_int(0, i);
            int tmp = p[i];
            p[i] = p[target];
            p[target] = tmp;
        }
    }
    static double perlin_interp(vec3 c[2][2][2], double u, double v, double w) {

        ////smooth 3 order
        auto uu = smooth_3order(u);
        auto vv = smooth_3order(v);
        auto ww = smooth_3order(w);

        // smooth 5order
        /*auto uu = smooth_5order(u);
        auto vv = smooth_5order(v);
        auto ww = smooth_5order(w);*/

        auto accum = 0.0;
        // nice code
        for (int i = 0; i < 2; i++)
            for (int j = 0; j < 2; j++)
                for (int k = 0; k < 2; k++)
                {
                    vec3 weight_v(u - i, v - j, w - k);
                    accum += (i * uu + (1 - i) * (1 - uu))
                        * (j * vv + (1 - j) * (1 - vv))
                        * (k * ww + (1 - k) * (1 - ww))
                        * dot(c[i][j][k], weight_v);
                }

        return accum;
    }
};