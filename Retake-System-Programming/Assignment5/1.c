void multi() {
    int i, j, k;
    ll t_sum;
    for (i = 0; i < MAX_LEN; ++i) {
        for (j = 0; j < MAX_LEN; ++j) {
            t_sum = 0;
            for (k = 0; k < MAX_LEN; ++k) {
                t_sum += (mat_a[i][k] * mat_b[k][j]);
            }
            mat_c[i][j] = t_sum;
        }
    }
}