/*
 * flash_read_write.h
 *
 *  Created on: Sep 13, 2025
 *      Author: rfidf
 */

#ifndef INC_FLASH_READ_WRITE_H_
#define INC_FLASH_READ_WRITE_H_


HAL_StatusTypeDef Flash_Write(flash_st *data,uint8_t page_number);
HAL_StatusTypeDef Flash_Erase(uint8_t page_number,uint8_t qty_pages);
void Flash_Read(flash_st *data,uint8_t page_number);





#endif /* INC_FLASH_READ_WRITE_H_ */
