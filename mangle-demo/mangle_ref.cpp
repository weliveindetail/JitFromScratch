extern "C" int *customIntAllocator(unsigned items);

int main(int argc, char **argv) {
    return customIntAllocator(argc)[0];
}
