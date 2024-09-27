FROM debian:bullseye

RUN apt-get update && apt-get upgrade -y
RUN apt-get install -y nginx wget zsh git curl vim lsof net-tools systemctl
RUN sh -c "$(wget https://raw.githubusercontent.com/ohmyzsh/ohmyzsh/master/tools/install.sh -O -)"

COPY ./conf/nginx.conf /etc/nginx/nginx.conf
COPY ./website/ /var/www/html/

EXPOSE 4242

ENTRYPOINT ["nginx", "-g", "daemon off;"]