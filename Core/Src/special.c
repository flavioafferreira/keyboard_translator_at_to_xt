/*
 * special.c
 *
 *  Created on: Sep 13, 2025
 *      Author: rfidf
 */
#include "special.h"
#include "stdio.h"
#include "math.h"
#include "string.h"
#include "stdint.h"
#include "fifo.h"

extern uint8_t ps2_ext;       // 0=normal, 1=E0 ativo, 2+=E1 sequência
extern uint8_t ps2_break_next; // se 1, próximo byte é break
extern uint8_t ps2_e1_swallow; // conta bytes para descartar após E1 (opcional)


extern fifo_queue q;
extern queue_item item;

#include <stdint.h>

// ===========================
// TABELA BASE: Set-2 (AT/PS2) → Set-1 (XT)
// Índice = byte do Set-2 (make sem E0/E1); valor = Set-1 (make).
// Onde valor==0, não há mapeamento direto aqui (ver Tabela E0).
// ===========================
const uint8_t set2_to_set1[256] = {
    // 0x00–0x0F
    [0x01] = 0x43, // F9
    [0x03] = 0x3F, // F5
    [0x04] = 0x3D, // F3
    [0x05] = 0x3B, // F1
    [0x06] = 0x3C, // F2
    [0x07] = 0x58, // F12
    [0x09] = 0x40, // F6
    [0x0A] = 0x41, // F8
    [0x0B] = 0x42, // F10
    [0x0C] = 0x57, // F11
    [0x0D] = 0x01, // Esc (Set1=0x01)
    [0x0E] = 0x29, // ` ~ (grave)

    // 0x10–0x1F
    [0x15] = 0x10, // Q
    [0x16] = 0x02, // 1
    [0x1A] = 0x2C, // Z
    [0x1B] = 0x1F, // S
    [0x1C] = 0x1E, // A
    [0x1D] = 0x11, // W
    [0x1E] = 0x03, // 2
    [0x1F] = 0x2F, // (International \ em alguns layouts; pode ficar 0)

    // 0x20–0x2F
    [0x21] = 0x2E, // C
    [0x22] = 0x2D, // X
    [0x23] = 0x20, // D
    [0x24] = 0x12, // E
    [0x25] = 0x05, // 4
    [0x26] = 0x04, // 3
    [0x29] = 0x39, // Space
    [0x2A] = 0x2F, // V
    [0x2B] = 0x21, // F
    [0x2C] = 0x14, // T
    [0x2D] = 0x13, // R
    [0x2E] = 0x06, // 5
    [0x2F] = 0x0C, // - _  (linha superior)

    // 0x30–0x3F
    [0x31] = 0x31, // N
    [0x32] = 0x30, // B
    [0x33] = 0x23, // H
    [0x34] = 0x22, // G
    [0x35] = 0x15, // Y
    [0x36] = 0x07, // 6
    [0x37] = 0x0D, // = +
    [0x38] = 0x0B, // 0
    [0x39] = 0x0A, // 9
    [0x3A] = 0x32, // M
    [0x3B] = 0x24, // J
    [0x3C] = 0x16, // U
    [0x3D] = 0x08, // 7
    [0x3E] = 0x09, // 8
    [0x3F] = 0x41, // F8 (já mapeado acima; alguns firmwares repetem F7=0x3A em Set2=0x83)

    // 0x40–0x4F
    [0x40] = 0x3A, // F7
    [0x41] = 0x33, // , <
    [0x42] = 0x25, // K
    [0x43] = 0x17, // I
    [0x44] = 0x18, // O
    [0x45] = 0x0B, // 0 (já acima, mantido)
    [0x46] = 0x0A, // 9 (já acima, mantido)
    [0x47] = 0x27, // ; :
    [0x48] = 0x28, // ' "
    [0x49] = 0x34, // . >
    [0x4A] = 0x35, // / ?
    [0x4B] = 0x26, // L
    [0x4C] = 0x1A, // [ {
    [0x4D] = 0x19, // P
    [0x4E] = 0x0C, // - _
    [0x4F] = 0x1B, // ] }

    // 0x50–0x5F
    [0x50] = 0x2B, // \ | (tecla do canto acima de Enter nos layouts US)
    [0x51] = 0x33, // , < (duplicado para alguns layouts; opcional)
    [0x52] = 0x28, // ' " (duplicado em alguns layouts)
    [0x54] = 0x1B, // ] } (duplicado)
    [0x55] = 0x0D, // = +
    [0x56] = 0x2B, // \ | (ISO <>\ pode usar 0)
    [0x58] = 0x3A, // CapsLock
    [0x59] = 0x36, // Right Shift
    [0x5A] = 0x1C, // Enter
    [0x5B] = 0x1A, // [ { (duplicado)
    [0x5D] = 0x2B, // \ | (duplicado)

    // 0x60–0x6F  (teclado numérico e locks)
    [0x66] = 0x0E, // Backspace
    [0x69] = 0x4F, // End  (E0 na verdade; mantenha via E0)
    [0x6B] = 0x4B, // Left (E0)
    [0x6C] = 0x47, // Home (E0)
    [0x70] = 0x52, // Insert (E0)
    [0x71] = 0x53, // Delete (E0)
    [0x72] = 0x50, // Down  (E0)
    [0x74] = 0x4D, // Right (E0)
    [0x75] = 0x48, // Up    (E0)
    [0x76] = 0x01, // Esc (duplicado)
    [0x77] = 0x45, // NumLock
    [0x78] = 0x46, // ScrollLock
    [0x79] = 0x4E, // Numpad +
    [0x7A] = 0x51, // PageDown (E0)
    [0x7B] = 0x4A, // Numpad -
    [0x7C] = 0x37, // Numpad *
    [0x7D] = 0x49, // PageUp (E0)
    [0x7E] = 0x46, // ScrollLock (dup)
    [0x7F] = 0x00, // (não usado)

    // 0x80–0x8F
    [0x83] = 0x3B, // F7/F1 variações (compat.) – ajustar conforme seu teclado

    // Modificadores (lado esquerdo)
    [0x11] = 0x38, // Left Alt
    [0x12] = 0x2A, // Left Shift
    [0x14] = 0x1D, // Left Ctrl

    // Linha superior restante
    [0x25] = 0x05, // 4 (já mapeado)
    [0x2E] = 0x06, // 5 (já mapeado)
    [0x36] = 0x07, // 6 (já mapeado)
    [0x3D] = 0x08, // 7 (já mapeado)
    [0x3E] = 0x09, // 8 (já mapeado)
    [0x46] = 0x0A, // 9 (já mapeado)
    [0x45] = 0x0B, // 0 (já mapeado)
    // (demais entradas permanecem 0)
};

// ===========================
// TABELA E0: Set-2 (prefixo E0) → Set-1 (XT)
// Use quando você detectou 0xE0 antes do código.
// ===========================
typedef struct { uint8_t set2; uint8_t set1; } e0_map_t;

static const e0_map_t set2E0_to_set1[] = {
    { 0x11, 0x38 }, // Right Alt  (E0 11) → Alt (XT não distingue; use 0x38)
    { 0x14, 0x1D }, // Right Ctrl (E0 14) → Ctrl (XT não distingue; use 0x1D)
    { 0x1F, 0x5B }, // Left GUI (Windows) → (sem equivalente no XT; 0 sugere ignorar)
    { 0x27, 0x5C }, // Right GUI → (sem equivalente no XT)
    { 0x2F, 0x5D }, // Menu → (sem equivalente no XT)
    { 0x4A, 0x35 }, // Numpad /  (em XT, normalmente não existe; usa 0x35 “/” principal)
    { 0x5A, 0x1C }, // Numpad Enter → Enter (XT não distingue)
    { 0x6B, 0x4B }, // Left Arrow
    { 0x6C, 0x47 }, // Home
    { 0x6D, 0x00 }, // (opcional)
    { 0x6E, 0x00 }, // (opcional)
    { 0x70, 0x52 }, // Insert
    { 0x71, 0x53 }, // Delete
    { 0x72, 0x50 }, // Down
    { 0x74, 0x4D }, // Right
    { 0x75, 0x48 }, // Up
    { 0x7A, 0x51 }, // Page Down
    { 0x7D, 0x49 }, // Page Up
    { 0x69, 0x4F }, // End
    // Encerrar com marcador (opcional)
    { 0x00, 0x00 }
};

// Busca simples na tabela E0:
static inline uint8_t translate_set2E0_to_set1(uint8_t b)
{
    for (unsigned i = 0; set2E0_to_set1[i].set2 != 0x00 || set2E0_to_set1[i].set1 != 0x00; ++i) {
        if (set2E0_to_set1[i].set2 == b) return set2E0_to_set1[i].set1;
    }
    return 0; // não mapeado
}


void print_float(float number) {
    int32_t mantissa = (int32_t)number;  // Parte inteira
    float intermediate = number - mantissa;
    int32_t expoente = (int32_t)(intermediate * 100000); // Parte decimal (5 casas)

    // Garantir que expoente seja positivo
    if (expoente < 0) expoente *= -1;

    // Impressão correta garantindo 5 dígitos no decimal
    printf("%ld.%05ld", mantissa, expoente);
}

void print_float_1dec(float number) {
    int32_t mantissa = (int32_t)number;  // Parte inteira
    float intermediate = number - mantissa;
    int32_t expoente = (int32_t)(intermediate * 100000); // Parte decimal (5 casas)

    // Garantir que expoente seja positivo
    if (expoente < 0) expoente *= -1;

    while (expoente >= 10) {
    	expoente /= 10;
        }
    // Impressão correta garantindo com 1 dígito
    printf("%ld.%ld", mantissa, expoente);
}


void sort(float* data, int n) {
    for (int i = 0; i < n - 1; ++i) {

        for (int j = 0; j < n - i - 1; ++j) {
            if (data[j] > data[j + 1]) {
                float temp = data[j];
                data[j] = data[j + 1];
                data[j + 1] = temp;
            }
        }
    }
}



int is_modifier_set2(uint16_t code16) {
    // LAlt=0x11, LShift=0x12, LCtrl=0x14, RCtrl=E0 14, RAlt=E0 11
    return (code16==0x11 || code16==0x12 || code16==0x14 ||   code16==0xE014 || code16==0xE011);
}



void ps2_feed_byte(uint8_t b)
{
    // 1) Prefixos
    if (b == 0xE0) { ps2_ext = 1; return; }

    if (b == 0xE1) {
        // Pause/Break: E1 14 77 E1 F0 14 F0 77 (make) — tratamos como "ignorar"
        ps2_ext = 2;
        ps2_e1_swallow = 7; // descartar 7 próximos bytes (bem prático)
        return;
    }

    if (ps2_ext == 2) {
        // Estamos no "modo E1": simplesmente engolimos N bytes
        if (ps2_e1_swallow) ps2_e1_swallow--;
        if (!ps2_e1_swallow) ps2_ext = 0;
        return; // não gera evento
    }

    if (b == 0xF0) { ps2_break_next = 1; return; }

    // 2) Montar "code16" só para detectar modificadores corretamente
    uint16_t code16 = (ps2_ext == 1) ? (uint16_t)(0xE000u | b) : (uint16_t)b;

    // 3) Se for BREAK
    if (ps2_break_next) {
        if (is_modifier_set2(code16)) {
            // Aqui você pode atualizar estado de modificadores (soltar)
            // modifier_set(code16, 0);
        }
        // Limpa estado e sai
        ps2_break_next = 0;
        ps2_ext = 0;
        return;
    }

    // 4) MAKE (pressionou)
    if (is_modifier_set2(code16)) {
        // Atualize estado de modificadores (pressionar)
        // modifier_set(code16, 1);
        ps2_ext = 0;
        return; // não enfileiramos modificadores como "tecla normal"
    }

    // 5) Tradução Set-2 -> Set-1 (XT), SEM prefixo E0
    uint8_t set1 = 0;
    if (ps2_ext == 0) {
        set1 = set2_to_set1[b];           // b é o byte Set-2 "puro"
    } else { // ps2_ext == 1
        set1 = translate_set2E0_to_set1(b); // traduz teclas E0
    }

    // 6) Se houver mapeamento, enfileira apenas o MAKE Set-1
    if (set1) {
        queue_item item;
        item.data_byte = set1;   // MAKE em Set-1 (XT)
        (void)enqueue(&q, item);
        // Se quiser também gerar BREAK em formato XT em algum momento:
        // queue_item br; br.data_byte = set1 | 0x80; enqueue(&q, br);
    }

    // 7) Limpa o estado de extensão
    ps2_ext = 0;
}






