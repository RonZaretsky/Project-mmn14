/**
 * @file assembler.h
 * @author Ron Zaretsky (ronz2512@icloud.com)
 * @brief This is the header file for the assembler module.
 * @version 0.1
 * @date 2023-07-04
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#ifndef __ASSEMBLER_H__
#define __ASSEMBLER_H__



/**
 * @brief This function is the main function of the assembler module.
 * 
 * @param file_count The number of files to assemble.
 * @param file_names The names of the files to assemble.
 * @return int 1 if the assembly was successful, 0 otherwise.
 */
int assemble(int file_count, char **file_names);


#endif /* __ASSEMBLER_H__ */