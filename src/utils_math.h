int calculate_size_with_padding(int size) {
    int paddingSize = size % 4;
    if (paddingSize > 0) {
        return size + (4 - paddingSize);
    }
    else {
        return size;
    }
}