import arche_compute as compute


if __name__ == '__main__':
    device = compute.create_device("metal")
    print(device.name())