FROM mysql/mysql-server:latest
ENV MYSQL_DATABASE datas
ENV MYSQL_ROOT_PASSWORD hyperpassword
ENV MYSQL_ROOT_HOST=%
COPY ./sql_scripts/create_tables.sql /docker-entrypoint-initdb.d/