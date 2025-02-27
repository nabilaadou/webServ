FROM debian:latest

RUN apt update && apt install -y nginx vim

COPY nginx.conf /etc/nginx/nginx.conf

RUN mv /var/www/html/index.nginx-debian.html /var/www/html/index.html

EXPOSE 80

CMD ["nginx", "-g", "daemon off;"]
