#ifndef SQL_H
#define SQL_H

typedef struct {
    char *server;
    char *user;
    char *password;
    char *database;
} UserData;

typedef struct {
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
} Sql;

bool myConnect(Sql* sql, UserData *ud){
    sql->conn = mysql_init(NULL);

    if (!mysql_real_connect(sql->conn, ud->server, ud->user, ud->password, ud->database, 0, NULL, 0))
    {
        fprintf(stderr, "%s\n", mysql_error(sql->conn));
	return false;
    }
    return true;
}

// Set second value to NULL if there is no array to input into
bool myQueryCh(Sql* sql, char* command, char* output){
    // printf("query command: %s\n", command);
    if (mysql_query(sql->conn, command))
    {
        fprintf(stderr, "QUERY FAIL Command: %s, Error: %s\n", command, mysql_error(sql->conn));
	return false;
    }
    myUseResultCh(sql, output);
    return true;
}

void myUseResultCh(Sql* sql, char* output){
    sql->res = mysql_use_result(sql->conn);
    size_t counter = 0;
    while ((sql->row = mysql_fetch_row(sql->res)) != NULL){
	if(output == NULL){
            printf("%s \n", sql->row[0]);
	}
	else {
	    strcpy(output, sql->row[0]);
	}
    }
}

// Set second value to NULL if there is no array to input into
bool myQueryChArr(Sql* sql, char* command, char** output){
    // printf("query command: %s\n", command);
    if (mysql_query(sql->conn, command))
    {
        fprintf(stderr, "QUERY FAIL Command: %s, Error: %s\n", command, mysql_error(sql->conn));
	return false;
    }
    myUseResultChArr(sql, output);
    return true;
}

void myUseResultChArr(Sql* sql, char** output){
    sql->res = mysql_use_result(sql->conn);
    size_t counter = 0;
    while ((sql->row = mysql_fetch_row(sql->res)) != NULL){
	if(output == NULL){
            printf("%s \n", sql->row[0]);
	}
	else {
	    // output[counter++] = sql->row[0];
	    strcpy(output[counter++] = malloc(strlen(sql->row[0]) + 1), sql->row[0]);
	    // printf("%d: %s ", counter - 1, output[counter - 1]);
	}
    }
}

// Set second value to NULL if there is no array to input into
bool myQueryFl(Sql* sql, char* command, float* output){
    // printf("query command: %s\n", command);
    if (mysql_query(sql->conn, command))
    {
        fprintf(stderr, "QUERY FAIL Command: %s, Error: %s\n", command, mysql_error(sql->conn));
	return false;
    }
    myUseResultFl(sql, output);
    return true;
}

void myUseResultFl(Sql* sql, float* output){
    sql->res = mysql_use_result(sql->conn);
    size_t counter = 0;
    while ((sql->row = mysql_fetch_row(sql->res)) != NULL){
	if(output == NULL){
            printf("%s \n", sql->row[0]);
	}
	else {
	    output[counter++] = atof(sql->row[0]);
	    // printf("%d: %f ", counter - 1, output[counter - 1]);
	}
    }
}
#endif
