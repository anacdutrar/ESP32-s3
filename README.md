# ESP32-S3 com sensor BMP180

Projeto desenvolvido com **ESP-IDF** para leitura de temperatura e pressão atmosférica utilizando um sensor **BMP180** conectado a uma placa **ESP32-S3**.

As medições são realizadas a cada dois segundos e exibidas no monitor serial em graus Celsius, pascals e hectopascals.

## Hardware utilizado

- ESP32-S3 DevKitC-1
- Sensor BMP180
- Cabos para conexão I²C

## Ligações

| BMP180 | ESP32-S3 |
|---|---|
| 3V3 | 3V3 |
| GND | GND |
| SDA | GPIO 9 |
| SCL | GPIO 8 |

O circuito também está disponível no arquivo [`diagram.json`](diagram.json) para simulação no Wokwi.

## Correção local do driver BMP180

O projeto utiliza como base o componente [`esp-idf-lib/bmp180`](https://components.espressif.com/components/esp-idf-lib/bmp180), versão 1.0.7.

Durante os testes, foi identificado um problema no driver original quando a leitura bruta de temperatura ultrapassava `32767`. O valor bruto `UT`, fornecido pelo BMP180 como um número de 16 bits sem sinal, era lido utilizando `int16_t`. Isso fazia o valor ser interpretado como negativo e provocava medições incorretas, observadas quando a temperatura calculada ultrapassava aproximadamente 51 °C.

A cópia local do componente corrige a leitura utilizando `uint16_t`:

```c
static esp_err_t bmp180_read_reg_16u(i2c_dev_t *dev, uint8_t reg, uint16_t *r)
{
    uint8_t d[] = { 0, 0 };

    CHECK(i2c_dev_read_reg(dev, reg, d, 2));
    *r = ((uint16_t)d[0] << 8) | d[1];

    return ESP_OK;
}
```

Essa alteração é aplicada somente à leitura da temperatura bruta. Os coeficientes de calibração que são definidos pelo datasheet como valores com sinal continuam utilizando `int16_t`.

## Estrutura do projeto

```text
.
├── components/
│   └── esp-idf-lib__bmp180/   # Cópia local do componente com a correção
├── main/
│   ├── CMakeLists.txt
│   ├── idf_component.yml
│   └── main.c
├── .gitignore
├── CMakeLists.txt
├── dependencies.lock
├── diagram.json
├── README.md
└── wokwi.toml
```

O `sdkconfig` e o diretório `build/` são gerados localmente e não devem ser versionados. O arquivo `dependencies.lock` é gerado pelo Component Manager e deve ser mantido no repositório para registrar as versões resolvidas.

## Requisitos

- ESP-IDF 6.1 ou versão compatível
- Python e ferramentas instaladas pelo ESP-IDF
- Cabo USB com suporte a dados

## Como compilar e executar

Abra um terminal com o ambiente do ESP-IDF carregado e cheque:
```bash
$env:IDF_TARGET
```

Caso o target esteja em outro esp32 e não esp2s, execute:

```bash
Remove-Item Env:IDF_TARGET
```
depois:

```bash
idf.py set-target esp32s3
idf.py build
```

Por fim, abra diagram.json com suas configurações do wokwi.


## Saída esperada

```text
BMP180 inicializado com sucesso!
SDA: GPIO 9 | SCL: GPIO 8
Temperatura: 24.50 C
Pressao: 101325 Pa
Pressao: 1013.25 hPa
```

## Simulação no Wokwi

Os arquivos [`diagram.json`](diagram.json) e [`wokwi.toml`](wokwi.toml) configuram a placa, o sensor e os artefatos gerados pela compilação. Compile o projeto antes de iniciar a simulação para que os arquivos referenciados dentro de `build/` existam.

## Créditos e licença do componente

O driver BMP180 modificado deriva do projeto [`esp-idf-lib`](https://github.com/esp-idf-lib/core) e mantém os avisos de copyright e a licença MIT do componente original. Consulte o arquivo `LICENSE` incluído junto à cópia local do componente.
