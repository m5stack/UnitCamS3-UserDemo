# UnitCAMS3-5MP UserDemo

## Build

### Fetch Dependencies

```bash
python ./fetch_repos.py
```

## Desktop Build

#### Tool Chains

```bash
sudo apt install build-essential cmake
```

#### Build

```bash
mkdir build && cd build
```
```bash
cmake .. && make -j8
```
#### Run

```bash
cd desktop && ./app_desktop_build
```

## IDF Build

#### Tool Chains

[ESP-IDF v5.1.4](https://docs.espressif.com/projects/esp-idf/en/v5.1.4/esp32s3/index.html)

#### Build

```bash
cd platforms/unitcam_s3_5mp
```

```bash
idf.py build
```

#### Flash

```bash
idf.py -p <YourPort> flash -b 1500000
```

##### Flash AssetPool

```bash
parttool.py --port <YourPort> write_partition --partition-name=assetpool --input "path/to/AssetPool.bin"
```

If you run desktop build before, you can found `AssetPool.bin` at 

`../../build/desktop/AssetPool.bin`.

