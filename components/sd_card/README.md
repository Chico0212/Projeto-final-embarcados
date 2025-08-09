# 💾 Biblioteca de Cartão SD para ESP32

Esta biblioteca oferece funções básicas para inicialização e manipulação de arquivos em um cartão SD utilizando a interface SPI no ESP32 com ESP-IDF.

Ideal para projetos que exigem armazenamento persistente, como placares de jogos, registros de sensores, arquivos de configuração e mais.

---

## 📌 Funcionalidades

- Montagem automática do cartão SD no sistema de arquivos
- Leitura e escrita de arquivos texto
- Atualização de pontuação com verificação de score máximo

---

## 🔧 Conexões SPI

| Sinal | GPIO Padrão |
|-------|-------------|
| MISO  | GPIO 2      |
| MOSI  | GPIO 15     |
| SCLK  | GPIO 14     |
| CS    | GPIO 13     |

---

## 📁 Ponto de Montagem

```c
#define SD_MNT_POINT "/files"
```

Todos os arquivos acessados usarão esse prefixo no caminho, por exemplo:

```c
"/files/score.txt"
```

---

## 🚀 Como Usar

### 1. Inicializar o cartão SD

```c
sd_card_init();
```

### 2. Escrever conteúdo em um arquivo

```c
write_file("/files/score.txt", "100");
```

### 3. Ler conteúdo de um arquivo

```c
char buffer[64];
read_file("/files/score.txt", buffer, sizeof(buffer));
printf("Conteúdo: %s\n", buffer);
```

### 4. Atualizar score (salvar novo se for maior)

```c
int atualizado = update_score("/files/score.txt", 120);
if (atualizado)
    printf("Novo recorde salvo!\n");
else
    printf("Pontuação anterior é maior.\n");
```

---

## 📚 Referência da API

| Função                        | Descrição |
|-------------------------------|-----------|
| `sd_card_init()`              | Inicializa e monta o cartão SD |
| `write_file(path, content)`   | Escreve conteúdo em um arquivo |
| `read_file(path, out, size)`  | Lê conteúdo de um arquivo para buffer |
| `update_score(path, score)`   | Atualiza placar somente se `score` for maior que o anterior |

---

## 🛠 Dependências

- ESP-IDF
- `esp_vfs_fat.h`
- `sdmmc_cmd.h`
- `esp_log.h`
- `sys/stat.h`

---

## 🧪 Exemplo Completo

```c
void app_main() {
    sd_card_init();

    write_file("/files/score.txt", "50");

    char buffer[32];
    read_file("/files/score.txt", buffer, sizeof(buffer));
    printf("Placar atual: %s\n", buffer);

    if (update_score("/files/score.txt", 120)) {
        printf("Novo recorde salvo.\n");
    }
}
```

---

## 📎 Licença

MIT ou compatível com seu projeto.
