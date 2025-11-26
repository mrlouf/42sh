#include "../incs/libft.h"

static int	intlen(int n)
{
	int		l;

	l = 1;
	if (n < 0)
		l++;
	while ((n / 10) != 0)
	{
		n = n / 10;
		l++;
	}
	return (l);
}

char	*ft_itoa(int n)
{
	char	*str;
	int		len;

	if (n == -2147483648)
		return (ft_strdup("-2147483648"));
	len = intlen(n);
	str = (char *)malloc(sizeof(char) * (len + 1));
	if (!str)
		return (NULL);
	str[len--] = '\0';
	if (n < 0)
	{
		str[0] = '-';
		n = -n;
	}
	while (len > 0)
	{
		str[len] = (n % 10) + '0';
		n = n / 10;
		len--;
	}
	if (str[len] != '-')
		str[len] = (n % 10) + '0';
	return (str);
}