/*
 * flash_read_write.c
 *
 *  Created on: Sep 13, 2025
 *      Author: rfidf
 */
#include "main.h"
#include "flash_read_write.h"
#include "string.h"
extern flash_st flash_variable;


HAL_StatusTypeDef Flash_Write(flash_st *data,uint8_t page_number)
{
    HAL_StatusTypeDef status = HAL_OK;
    FLASH_EraseInitTypeDef eraseInitStruct;
    uint32_t PageError = 0;
    uint32_t flashAddress = (page_number * FLASH_PAGE_SIZE)+0x08000000; //2kbytes/page

    // Destrava a Flash
    HAL_FLASH_Unlock();

    // Configuração para apagar a página onde os dados serão gravados
    eraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
    eraseInitStruct.Page = 14;
    eraseInitStruct.NbPages = 1;

    status = HAL_FLASHEx_Erase(&eraseInitStruct, &PageError);
    if (status != HAL_OK)
    {
        HAL_FLASH_Lock();
        return status;
    }

    // Escreve a estrutura inteira, 8 bytes por vez
    uint64_t *data_ptr = (uint64_t *)data;
    for (uint32_t i = 0; i < sizeof(flash_st) / sizeof(uint64_t); i++)
    {
        status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, flashAddress, data_ptr[i]);
        if (status != HAL_OK)
        {
            HAL_FLASH_Lock();
            return status;
        }
        flashAddress += sizeof(uint64_t);
    }

    // Trava a Flash novamente
    HAL_FLASH_Lock();

    return HAL_OK;
}

HAL_StatusTypeDef Flash_Erase(uint8_t page_number,uint8_t qty_pages){
    HAL_StatusTypeDef status;
    FLASH_EraseInitTypeDef eraseInitStruct;
    uint32_t PageError = 0;

    // Desbloqueia a memória flash para acesso de gravação
    HAL_FLASH_Unlock();

    eraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
    eraseInitStruct.NbPages     = qty_pages;
    //FLASH_PAGE_NB
    eraseInitStruct.Page = page_number;

    // Apaga a página da flash
    status = HAL_FLASHEx_Erase(&eraseInitStruct, &PageError);
    if (status != HAL_OK)
    {
        HAL_FLASH_Lock();
        return status;
    }


    // Bloqueia a memória flash após a gravação
    HAL_FLASH_Lock();

    return HAL_OK;
}




void Flash_Read(flash_st *data,uint8_t page_number)
{
	uint32_t flashAddress_data=(page_number*FLASH_PAGE_SIZE)+0x08000000;
    memcpy(data, (void *)flashAddress_data, sizeof(flash_st));
}


