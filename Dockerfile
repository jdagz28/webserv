FROM debian:bullseye

RUN apt-get update && apt-get upgrade -y
RUN apt-get install -y nginx wget zsh git curl vim
RUN sh -c "$(wget https://raw.githubusercontent.com/ohmyzsh/ohmyzsh/master/tools/install.sh -O -)"

COPY ./conf/default.conf /etc/nginx/nginx.conf
COPY ./website/ /var/www/html/

EXPOSE 4242

CMD ["nginx", "-g", "daemon off;"]