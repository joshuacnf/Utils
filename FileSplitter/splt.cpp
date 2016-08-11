#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <string>

#define M1 0x100000ULL 
#define M128 0x8000000ULL

typedef unsigned long long ull;

char *buffer = 0;

int main(int argc, char *argv[])
{
    printf("\n");
    if (argc == 4 && !strcmp(argv[1], "-s"))
    {
	FILE *in = fopen(argv[2], "rb");
	
	if (!in)
	{
	    fprintf(stderr, "Failed to open %s.\n", argv[2]);
	    return 1;
	}
	
	fseek(in, 0, SEEK_END);
	ull file_size = ftell(in);
	ull part_num = std::stoull(argv[3]);
	
	if (part_num <= 0 || part_num > file_size)
	{
	    fprintf(stderr, "Invalid number of parts.\n");
	    return 2;
	}
	
	printf("%s (%.2lf MB) will be split into %llu parts:\n\n", 
	       argv[2], file_size / (double)M1, part_num);

	ull part_size = file_size / part_num + 1;
	buffer = (char*)malloc(part_size);

	FILE *out; ull file_no = 0, len = 0;
	char output_file[strlen(argv[2]) + strlen(argv[3]) + 3];
	fseek(in, 0, SEEK_SET);
	    
	while ((len = fread(buffer, 1, part_size, in)))
	{
	    snprintf(output_file, sizeof(output_file), "%s_%llu", argv[2], file_no);
	    printf("Writing to %s\n", output_file);

	    out = fopen(output_file, "wb");
	    fwrite(buffer, 1, len, out);
	    fclose(out);
	    
	    file_no++;
	}

	fclose(in);
	free(buffer);

	printf("\nDone.\n\n");
    }
    else if (argc == 3 && !strcmp(argv[1], "-m"))
    {
	FILE *out = fopen(argv[2], "wb");

	FILE *in; ull file_no = 0;
	char input_file[strlen(argv[2]) + 20];

	snprintf(input_file, sizeof(input_file), "%s_%llu", argv[2], file_no);
	buffer = (char*)malloc(M128);
	
	ull file_size = 0;
	while ((in = fopen(input_file, "rb")))
	{
	    printf("Merging %s\n", input_file);
		
	    ull len;
	    while((len = fread(buffer, 1, M128, in)))
	    {
		fwrite(buffer, 1, len, out);
		file_size += len;
	    }
	    
	    file_no++;
	    snprintf(input_file, sizeof(input_file), "%s_%llu", argv[2], file_no);
	    fclose(in);
	}
	
	fclose(out);
	free(buffer);

	printf("\n%llu parts merged into %s (%.2lf MB)\n\n", file_no, argv[2], file_size / (double)M1);
    }
    else
    {
	fprintf(stderr, "Usage:\n");
	fprintf(stderr, "To split a file: -s <file name> <number of parts>\n");
	fprintf(stderr, "To merge splitted files: -m <file name>\n");
	return 3;
    }
    
    return 0;
}
